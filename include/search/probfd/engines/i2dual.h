#ifndef MDPS_ENGINES_I2DUAL_H
#define MDPS_ENGINES_I2DUAL_H

#include "probfd/engines/engine.h"

#include "probfd/analysis_objectives/analysis_objective.h"
#include "probfd/analysis_objectives/goal_probability_objective.h"

#include "probfd/heuristics/occupation_measure/occupation_measure_heuristic.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/utils/logging.h"

#include "probfd/globals.h"
#include "probfd/progress_report.h"
#include "probfd/value_type.h"

#include "lp/lp_solver.h"

#include "utils/timer.h"

#include "global_state.h"

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
public:
    explicit I2Dual(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<Action>* action_id_map,
        engine_interfaces::RewardFunction<State, Action>* reward_function,
        value_utils::IntervalValue reward_bound,
        engine_interfaces::TransitionGenerator<Action>* transition_generator,
        ProgressReport* progress,
        engine_interfaces::StateEvaluator<State>* heuristic,
        bool hpom_enabled,
        bool incremental_updates,
        lp::LPSolverType solver_type)
        : MDPEngine<State, Action>(
              state_id_map,
              action_id_map,
              reward_function,
              reward_bound,
              transition_generator)
        , progress_(progress)
        , heuristic_(heuristic)
        , hpom_enabled_(hpom_enabled)
        , incremental_hpom_updates_(incremental_updates)
        , lp_solver_(solver_type)
    {
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

    virtual value_type::value_t solve(const State& state) override
    {
        statistics_ = Statistics();

        using namespace analysis_objectives;

        logging::out << "Initializing I2-Dual..." << std::endl;

        if (std::dynamic_pointer_cast<GoalProbabilityObjective>(
                g_analysis_objective) == nullptr) {
            logging::err
                << "I2-Dual currently only supports goal probability analysis"
                << std::endl;
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }

        if (hpom_enabled_ && (::has_axioms() || ::has_conditional_effects())) {
            logging::err
                << "hpom doesn't support axioms and conditional effects!"
                << std::endl;
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }

        storage::PerStateStorage<IDualData> idual_data;

        next_lp_var_ = 0;
        next_lp_constr_id_ = 0;

        statistics_.idual_timer_.resume();

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

        objective_ = value_type::one;

        {
            const StateID init_id = this->get_state_id(state);
            if (!evaluate_state(state, idual_data[init_id])) {
                frontier.push_back(init_id);
            }
        }

        while (!frontier.empty()) {
            progress_->operator()();
            // std::cout << "[v=" << temp << "]" << std::endl;

            update_hpom_constraints_expanded(idual_data, frontier);

            // Expand each state in frontier
            statistics_.expansions_ += frontier.size();
            unsigned start_new_states = frontier_candidates.size();

            for (const StateID& state_id : frontier) {
                IDualData& state_data = idual_data[state_id];
                assert(state_data.is_frontier());

                // state is expanded -> no longer in frontier -> remove it from
                // LP's objective function
                // if (state_id != istate) {
                //     lp_solver_.set_constraint_lower_bound(
                //           state_data.constraint, 0);
                // }

                if (!hpom_enabled_) {
                    for (const auto& [prob, var_id] : state_data.incoming) {
                        const double amount = state_data.estimate * prob;
                        obj_coef[var_id] -= amount;
                        assert(obj_coef[var_id] >= -value_type::g_epsilon);
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

            statistics_.lp_solver_timer_.resume();
            lp_solver_.solve();
            statistics_.lp_solver_timer_.stop();

            assert(lp_solver_.has_optimal_solution());
            objective_ = lp_solver_.get_objective_value();
            std::vector<double> solution = lp_solver_.extract_solution();

            unsigned j = 0;
            for (unsigned i = 0; i < frontier_candidates.size(); i++) {
                const auto& state_data = idual_data[frontier_candidates[i]];

                for (const auto& [_, var_id] : state_data.incoming) {
                    if (solution[var_id] > value_type::g_epsilon) {
                        frontier.push_back(frontier_candidates[i]);
                        goto continue_outer;
                    }
                }

                if (i != j) {
                    frontier_candidates[j] = frontier_candidates[i];
                }
                j++;

            continue_outer:;
            }

            while (frontier_candidates.size() > j) {
                frontier_candidates.pop_back();
            }

            lp_solver_.clear_temporary_constraints();

            statistics_.iterations_++;
            statistics_.open_states_ =
                frontier_candidates.size() + frontier.size();
        }

        statistics_.idual_timer_.stop();

        statistics_.num_lp_vars_ = next_lp_var_;
        statistics_.num_lp_constraints_ = next_lp_constr_id_;

        return objective_;
    }

private:
    bool evaluate_state(const State& state, IDualData& data)
    {
        assert(data.is_new());

        auto x = this->get_state_reward(state);
        if ((bool)x) {
            data.set_terminal((value_type::value_t)x);
            return true;
        }

        x = heuristic_->operator()(state);
        if ((bool)x) {
            data.set_terminal(0);
            return true;
        }

        data.open(next_lp_constr_id_++, (value_type::value_t)x);
        return false;
    }

    void prepare_lp()
    {
        // setup empty LP
        std::vector<lp::LPVariable> vars;
        std::vector<lp::LPConstraint> constr;
        prepare_hpom(vars);

        next_lp_var_ = vars.size();
        next_lp_constr_id_ = constr.size();

        constr.emplace_back(-lp_solver_.get_infinity(), 1);
        lp_solver_.load_problem(lp::LPObjectiveSense::MAXIMIZE, vars, constr);
    }

    void prepare_hpom(std::vector<lp::LPVariable>& vars)
    {
        if (!hpom_enabled_) {
            return;
        }

        statistics_.hpom_timer_.resume();
        occupation_measure_heuristic::ProjectionOccupationMeasureHeuristic::
            generate_hpom_lp(
                lp_solver_,
                vars,
                hpom_constraints_,
                offset_,
                true);
        statistics_.hpom_num_vars_ = vars.size();
        statistics_.hpom_num_constraints_ = hpom_constraints_.size();

        // constraints.insert(
        //     constraints.end(), hpom_constraints_.begin(),
        //     hpom_constraints_.end());

        statistics_.hpom_timer_.stop();
    }

    void update_hpom_constraints_expanded(
        storage::PerStateStorage<IDualData>& data,
        const std::vector<StateID>& expanded)
    {
        if (!hpom_enabled_ || !incremental_hpom_updates_) {
            return;
        }

        if (hpom_initialized_) {
            statistics_.hpom_timer_.resume();
            for (const StateID state_id : expanded) {
                remove_fringe_state_from_hpom(
                    this->lookup_state(state_id),
                    data[state_id],
                    hpom_constraints_);
            }

            statistics_.hpom_timer_.stop();
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

        statistics_.hpom_timer_.resume();

        if (incremental_hpom_updates_) {
            for (size_t i = start; i < frontier.size(); ++i) {
                const StateID& state_id = frontier[i];
                State s = this->lookup_state(state_id);
                add_fringe_state_to_hpom(s, data[state_id], hpom_constraints_);
            }

            lp_solver_.add_temporary_constraints(hpom_constraints_);
        } else {
            std::vector<lp::LPConstraint> constraints(hpom_constraints_);
            for (size_t i = 0; i < frontier.size(); ++i) {
                const StateID& state_id = frontier[i];
                State s = this->lookup_state(state_id);
                add_fringe_state_to_hpom(s, data[state_id], constraints);
            }

            lp_solver_.add_temporary_constraints(constraints);
        }

        statistics_.hpom_timer_.stop();
    }

    void remove_fringe_state_from_hpom(
        const State& state,
        const IDualData& data,
        std::vector<lp::LPConstraint>& constraints) const
    {
        for (size_t var = 0; var != g_variable_domain.size(); ++var) {
            const int val = state[var];
            lp::LPConstraint& c = constraints[offset_[var] + val];
            for (const auto& om : data.incoming) {
                c.remove(om.second);
            }
        }
    }

    void add_fringe_state_to_hpom(
        const State& state,
        const IDualData& data,
        std::vector<lp::LPConstraint>& constraints) const
    {
        for (size_t var = 0; var != g_variable_domain.size(); ++var) {
            const int val = state[var];
            lp::LPConstraint& c = constraints[offset_[var] + val];
            for (const auto& om : data.incoming) {
                c.insert(om.second, om.first);
            }
        }
    }

    ProgressReport* progress_;

    engine_interfaces::StateEvaluator<State>* heuristic_;

    const bool hpom_enabled_;
    const bool incremental_hpom_updates_;

    lp::LPSolver lp_solver_;

    size_t next_lp_var_ = 0;
    size_t next_lp_constr_id_ = 0;

    bool hpom_initialized_ = false;
    std::vector<int> offset_;
    std::vector<lp::LPConstraint> hpom_constraints_;

    Statistics statistics_;

    value_type::value_t objective_;

    std::vector<Action> aops_;
    Distribution<StateID> succs_;
};

} // namespace i2dual
} // namespace engines
} // namespace probfd

#endif // __I2DUAL_H__