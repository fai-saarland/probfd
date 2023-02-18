#ifndef PROBFD_ENGINES_I2DUAL_H
#define PROBFD_ENGINES_I2DUAL_H

#include "probfd/engines/engine.h"

#include "probfd/cost_models/maxprob_cost_model.h"

#include "probfd/heuristics/occupation_measure/occupation_measure_heuristic.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/progress_report.h"
#include "probfd/value_type.h"

#include "lp/lp_solver.h"

#include "utils/timer.h"

#include "task_proxy.h"
#include "task_utils/task_properties.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/tasks/root_task.h"

#include <memory>
#include <vector>

namespace options {
class Options;
class OptionParser;
} // namespace options

class StateRegistry;

namespace probfd {
namespace engines {
namespace i2dual {

struct IDualData {
    enum IDualStateStatus : uint8_t { NEW, FRONTIER, TERMINAL, CLOSED };

    std::vector<std::pair<double, unsigned>> incoming;
    double estimate = -1;
    unsigned constraint = -1;
    IDualStateStatus status = NEW;

    bool is_new() const { return status == NEW; }
    bool is_terminal() const { return status == TERMINAL; }
    bool is_frontier() const { return status == FRONTIER; }

    void open(unsigned c, double est)
    {
        status = FRONTIER;
        constraint = c;
        estimate = est;
    }

    void set_terminal(double val)
    {
        status = TERMINAL;
        estimate = val;
    }

    void close()
    {
        status = CLOSED;
        std::vector<std::pair<double, unsigned>>().swap(incoming);
    }
};

struct Statistics {
    utils::Timer idual_timer_ = utils::Timer(true);
    utils::Timer lp_solver_timer_ = utils::Timer(true);
    utils::Timer hpom_timer_ = utils::Timer(true);

    unsigned long long iterations_ = 0;
    unsigned long long expansions_ = 0;
    unsigned long long open_states_ = 0;
    unsigned long long num_lp_vars_ = 0;
    unsigned long long num_lp_constraints_ = 0;
    unsigned long long hpom_num_vars_ = 0;
    unsigned long long hpom_num_constraints_ = 0;

    void print(std::ostream& out) const
    {
        out << "Engine I2-Dual statistics:" << std::endl;
        out << "  Actual solver time: " << idual_timer_() << std::endl;
        out << "  Iterations: " << iterations_ << std::endl;
        out << "  Expansions: " << expansions_ << std::endl;
        out << "  Open states: " << open_states_ << std::endl;
        out << "  LP Variables: " << num_lp_vars_ << std::endl;
        out << "  LP Constraints: " << num_lp_constraints_ << std::endl;
        out << "  LP Timer: " << lp_solver_timer_() << std::endl;
        out << "  hPom LP Variables: " << hpom_num_vars_ << std::endl;
        out << "  hPom LP Constraints: " << hpom_num_constraints_ << std::endl;
        out << "  hPom Overhead: " << hpom_timer_() << std::endl;
    }
};

template <typename State, typename Action>
class I2Dual : public MDPEngine<State, Action> {
    ProbabilisticTaskProxy task_proxy;

    engine_interfaces::Evaluator<State>* heuristic_;

    ProgressReport* progress_;

    const bool hpom_enabled_;
    const bool incremental_hpom_updates_;

    lp::LPSolver lp_solver_;

    size_t next_lp_var_ = 0;
    size_t next_lp_constr_id_ = 0;

    bool hpom_initialized_ = false;
    std::vector<int> offset_;
    named_vector::NamedVector<lp::LPConstraint> hpom_constraints_;

    Statistics statistics_;

    value_t objective_;

    std::vector<Action> aops_;
    Distribution<StateID> succs_;

public:
    I2Dual(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<Action>* action_id_map,
        engine_interfaces::TransitionGenerator<Action>* transition_generator,
        engine_interfaces::CostFunction<State, Action>* cost_function,
        engine_interfaces::Evaluator<State>* heuristic,
        ProgressReport* progress,
        bool hpom_enabled,
        bool incremental_updates,
        lp::LPSolverType solver_type)
        : MDPEngine<State, Action>(
              state_id_map,
              action_id_map,
              transition_generator,
              cost_function)
        , task_proxy(*tasks::g_root_task)
        , heuristic_(heuristic)
        , progress_(progress)
        , hpom_enabled_(hpom_enabled)
        , incremental_hpom_updates_(incremental_updates)
        , lp_solver_(solver_type)
    {
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

    virtual value_t solve(const State& state) override
    {
        statistics_ = Statistics();

        std::cout << "Initializing I2-Dual..." << std::endl;

        if (!std::dynamic_pointer_cast<cost_models::MaxProbCostModel>(
                g_cost_model)) {
            std::cerr
                << "I2-Dual currently only supports goal probability analysis"
                << std::endl;
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }

        if (hpom_enabled_) {
            ::task_properties::verify_no_axioms(task_proxy);
            probfd::task_properties::verify_no_conditional_effects(task_proxy);
        }

        storage::PerStateStorage<IDualData> idual_data;

        next_lp_var_ = 0;
        next_lp_constr_id_ = 0;

        utils::TimerScope scope(statistics_.idual_timer_);

        prepare_lp();

        progress_->register_value("v", [this]() { return objective_; });

        const double infinity = lp_solver_.get_infinity();

        // Data structures for refining LP
        lp::LPVariable dummy_variable(0, infinity, 0);
        lp::LPConstraint dummy_constraint(-infinity, 0);
        std::vector<int> var_constraint_ids;
        std::vector<double> var_constraint_coefs;
        std::vector<double> obj_coef(next_lp_var_, 0);

        // IDual data structures
        std::vector<StateID> frontier;
        std::vector<StateID> frontier_candidates;

        objective_ = -1_vt;

        {
            const StateID init_id = this->get_state_id(state);
            if (!evaluate_state(state, idual_data[init_id])) {
                frontier.push_back(init_id);
            }
        }

        while (!frontier.empty()) {
            progress_->operator()();

            update_hpom_constraints_expanded(idual_data, frontier);

            // Expand each state in frontier
            statistics_.expansions_ += frontier.size();
            unsigned start_new_states = frontier_candidates.size();

            for (StateID state_id : frontier) {
                IDualData& state_data = idual_data[state_id];
                assert(state_data.is_frontier());

                if (!hpom_enabled_) {
                    for (const auto& [prob, var_id] : state_data.incoming) {
                        const double amount = state_data.estimate * prob;
                        obj_coef[var_id] -= amount;
                        assert(obj_coef[var_id] >= -g_epsilon);
                        lp_solver_.set_objective_coefficient(
                            var_id,
                            std::abs(obj_coef[var_id]));
                    }
                }

                state_data.close();

                // generate transitions
                aops_.clear();
                this->generate_applicable_ops(state_id, aops_);

                for (const Action& act : aops_) {
                    succs_.clear();
                    this->generate_successors(state_id, act, succs_);
                    succs_.make_unique();

                    if (succs_.is_dirac(state_id)) {
                        continue;
                    }

                    unsigned lp_var_id = next_lp_var_++;

                    double p_self = 0;
                    for (const auto& [succ_id, prob] : succs_) {
                        if (succ_id == state_id) {
                            p_self += prob;
                            continue;
                        }

                        IDualData& succ_data = idual_data[succ_id];
                        if (succ_data.is_new() &&
                            !evaluate_state(
                                this->lookup_state(succ_id),
                                succ_data)) {
                            lp_solver_.add_constraint(dummy_constraint);
                            frontier_candidates.push_back(succ_id);
                        }

                        if (!succ_data.is_terminal()) {
                            assert(succ_data.constraint < next_lp_constr_id_);
                            var_constraint_ids.push_back(succ_data.constraint);
                            var_constraint_coefs.push_back(-prob);
                        }

                        if (succ_data.is_frontier()) {
                            succ_data.incoming.emplace_back(prob, lp_var_id);
                        }

                        if (succ_data.is_terminal() ||
                            (!hpom_enabled_ && succ_data.is_frontier())) {
                            dummy_variable.objective_coefficient +=
                                prob * succ_data.estimate;
                        }
                    }

                    var_constraint_ids.push_back(state_data.constraint);
                    var_constraint_coefs.push_back(1.0 - p_self);
                    obj_coef.push_back(dummy_variable.objective_coefficient);

                    lp_solver_.add_variable(
                        dummy_variable,
                        var_constraint_ids,
                        var_constraint_coefs);

                    dummy_variable.objective_coefficient = 0.0;
                    var_constraint_ids.clear();
                    var_constraint_coefs.clear();
                }
            }

            frontier.clear();

            update_hpom_constraints_frontier(
                idual_data,
                frontier_candidates,
                start_new_states);

            {
                utils::TimerScope lp_scope(statistics_.lp_solver_timer_);
                lp_solver_.solve();
            }

            assert(lp_solver_.has_optimal_solution());
            objective_ = -lp_solver_.get_objective_value();
            std::vector<double> solution = lp_solver_.extract_solution();

            // Push frontier candidates and remove them
            std::erase_if(frontier_candidates, [&, this](StateID state_id) {
                for (const auto& [_, var_id] : idual_data[state_id].incoming) {
                    if (solution[var_id] > g_epsilon) {
                        frontier.push_back(state_id);
                        return true;
                    }
                }

                return false;
            });

            lp_solver_.clear_temporary_constraints();

            statistics_.iterations_++;
            statistics_.open_states_ =
                frontier_candidates.size() + frontier.size();
        }

        statistics_.num_lp_vars_ = next_lp_var_;
        statistics_.num_lp_constraints_ = next_lp_constr_id_;

        return objective_;
    }

private:
    bool evaluate_state(const State& state, IDualData& data)
    {
        assert(data.is_new());

        const TerminationInfo term_info = this->get_termination_info(state);
        if (term_info.is_goal_state()) {
            data.set_terminal(-term_info.get_cost());
            return true;
        }

        const EvaluationResult eval = heuristic_->evaluate(state);
        if (eval.is_unsolvable()) {
            data.set_terminal(0);
            return true;
        }

        data.open(next_lp_constr_id_++, -eval.get_estimate());
        return false;
    }

    void prepare_lp()
    {
        // setup empty LP
        named_vector::NamedVector<lp::LPVariable> vars;
        named_vector::NamedVector<lp::LPConstraint> constr;
        prepare_hpom(vars);

        next_lp_var_ = vars.size();
        next_lp_constr_id_ = constr.size();

        constr.emplace_back(-lp_solver_.get_infinity(), 1);
        lp_solver_.load_problem(lp::LinearProgram(
            lp::LPObjectiveSense::MAXIMIZE,
            std::move(vars),
            std::move(constr),
            lp_solver_.get_infinity()));
    }

    void prepare_hpom(named_vector::NamedVector<lp::LPVariable>& vars)
    {
        using namespace heuristics::occupation_measure_heuristic;

        if (!hpom_enabled_) {
            return;
        }

        utils::TimerScope scope(statistics_.hpom_timer_);
        ProjectionOccupationMeasureHeuristic::generate_hpom_lp(
            ProbabilisticTaskProxy(*tasks::g_root_task),
            lp_solver_,
            vars,
            hpom_constraints_,
            offset_,
            true);
        statistics_.hpom_num_vars_ = vars.size();
        statistics_.hpom_num_constraints_ = hpom_constraints_.size();
    }

    void update_hpom_constraints_expanded(
        storage::PerStateStorage<IDualData>& data,
        const std::vector<StateID>& expanded)
    {
        if (!hpom_enabled_ || !incremental_hpom_updates_) {
            return;
        }

        if (hpom_initialized_) {
            utils::TimerScope scope(statistics_.hpom_timer_);
            for (const StateID state_id : expanded) {
                remove_fringe_state_from_hpom(
                    this->lookup_state(state_id),
                    data[state_id],
                    hpom_constraints_);
            }
        }

        hpom_initialized_ = true;
    }

    void update_hpom_constraints_frontier(
        storage::PerStateStorage<IDualData>& data,
        const std::vector<StateID>& frontier,
        const unsigned start)
    {
        if (!hpom_enabled_) {
            return;
        }

        utils::TimerScope scope(statistics_.hpom_timer_);

        size_t i = incremental_hpom_updates_ ? start : 0;

        for (; i < frontier.size(); ++i) {
            StateID state_id = frontier[i];
            State s = this->lookup_state(state_id);
            add_fringe_state_to_hpom(s, data[state_id], hpom_constraints_);
        }

        lp_solver_.add_temporary_constraints(hpom_constraints_);
    }

    void remove_fringe_state_from_hpom(
        const State& state,
        const IDualData& data,
        named_vector::NamedVector<lp::LPConstraint>& constraints) const
    {
        for (VariableProxy var : task_proxy.get_variables()) {
            const int val = state[var].get_value();
            lp::LPConstraint& c = constraints[offset_[var.get_id()] + val];
            for (const auto& om : data.incoming) {
                c.remove(om.second);
            }
        }
    }

    void add_fringe_state_to_hpom(
        const State& state,
        const IDualData& data,
        named_vector::NamedVector<lp::LPConstraint>& constraints) const
    {
        for (VariableProxy var : task_proxy.get_variables()) {
            const int val = state[var].get_value();
            lp::LPConstraint& c = constraints[offset_[var.get_id()] + val];
            for (const auto& om : data.incoming) {
                c.insert(om.second, om.first);
            }
        }
    }
};

} // namespace i2dual
} // namespace engines
} // namespace probfd

#endif // __I2DUAL_H__