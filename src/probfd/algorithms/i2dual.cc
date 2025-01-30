#include "probfd/algorithms/i2dual.h"

#include "probfd/algorithms/utils.h"

#include "probfd/occupation_measures/hpom_constraints.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/utils/guards.h"

#include "probfd/progress_report.h"

#include "probfd/evaluator.h"
#include "probfd/mdp.h"

#include "downward/task_utils/task_properties.h"

#include "downward/utils/countdown_timer.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <utility>

namespace probfd::algorithms::i2dual {

void I2Dual::Statistics::print(std::ostream& out) const
{
    out << "Algorithm I2-Dual statistics:" << std::endl;
    out << "  Actual solver time: " << idual_timer() << std::endl;
    out << "  Iterations: " << iterations << std::endl;
    out << "  Expansions: " << expansions << std::endl;
    out << "  Open states: " << open_states << std::endl;
    out << "  LP Variables: " << num_lp_vars << std::endl;
    out << "  LP Constraints: " << num_lp_constraints << std::endl;
    out << "  LP Timer: " << lp_solver_timer() << std::endl;
    out << "  hPom LP Variables: " << hpom_num_vars << std::endl;
    out << "  hPom LP Constraints: " << hpom_num_constraints << std::endl;
    out << "  hPom Overhead: " << hpom_timer() << std::endl;
}

struct I2Dual::IDualData {
    enum { NEW, FRONTIER, TERMINAL, CLOSED };

    std::vector<std::pair<double, unsigned>> incoming;
    double estimate = -1.0;
    unsigned constraint = std::numeric_limits<unsigned>::max();
    uint8_t status = NEW;

    [[nodiscard]]
    bool is_new() const
    {
        return status == NEW;
    }
    [[nodiscard]]
    bool is_terminal() const
    {
        return status == TERMINAL;
    }
    [[nodiscard]]
    bool is_frontier() const
    {
        return status == FRONTIER;
    }

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

I2Dual::I2Dual(
    std::shared_ptr<ProbabilisticTask> task,
    std::shared_ptr<FDRCostFunction> task_cost_function,
    bool hpom_enabled,
    bool incremental_updates,
    lp::LPSolverType solver_type,
    double fp_epsilon)
    : task_proxy_(*task)
    , task_cost_function_(std::move(task_cost_function))
    , hpom_enabled_(hpom_enabled)
    , incremental_hpom_updates_(incremental_updates)
    , lp_solver_(solver_type)
    , fp_epsilon_(fp_epsilon)
{
}

void I2Dual::print_statistics(std::ostream& out) const
{
    statistics_.print(out);
}

Interval I2Dual::solve(
    FDRMDP& mdp,
    FDREvaluator& heuristic,
    const State& initial_state,
    ProgressReport progress,
    double max_time)
{
    utils::CountdownTimer timer(max_time);

    statistics_ = Statistics();

    std::cout << "Initializing I2-Dual..." << std::endl;

    if (hpom_enabled_) {
        ::task_properties::verify_no_axioms(task_proxy_);
        probfd::task_properties::verify_no_conditional_effects(task_proxy_);
    }

    storage::PerStateStorage<IDualData> idual_data;

    next_lp_var_ = 0;
    next_lp_constr_id_ = 0;

    TimerScope scope(statistics_.idual_timer);

    prepare_lp();

    progress.register_bound("v", [this]() {
        return Interval(objective_, INFINITE_VALUE);
    });

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
        const StateID init_id = mdp.get_state_id(initial_state);
        if (!evaluate_state(
                mdp,
                heuristic,
                initial_state,
                idual_data[init_id])) {
            frontier.push_back(init_id);
        }
    }

    while (!frontier.empty()) {
        progress.print();
        timer.throw_if_expired();

        update_hpom_constraints_expanded(mdp, idual_data, frontier);

        // Expand each initial_state in frontier
        statistics_.expansions += frontier.size();
        unsigned start_new_states = frontier_candidates.size();

        for (StateID state_id : frontier) {
            timer.throw_if_expired();

            IDualData& state_data = idual_data[state_id];
            assert(state_data.is_frontier());

            if (!hpom_enabled_) {
                for (const auto& [prob, var_id] : state_data.incoming) {
                    const double amount = state_data.estimate * prob;
                    obj_coef[var_id] -= amount;
                    assert(obj_coef[var_id] >= -fp_epsilon_);
                    lp_solver_.set_objective_coefficient(
                        var_id,
                        std::abs(obj_coef[var_id]));
                }
            }

            state_data.close();

            // generate transitions
            const State state = mdp.get_state(state_id);

            ClearGuard _guard_a(aops_);
            mdp.generate_applicable_actions(state, aops_);

            for (const OperatorID& act : aops_) {
                ClearGuard _guard_s(succs_);

                mdp.generate_action_transitions(state, act, succs_);

                if (succs_.is_dirac(state_id)) {
                    continue;
                }

                ClearGuard _guard(var_constraint_ids, var_constraint_coefs);

                unsigned lp_var_id = next_lp_var_++;

                double p_self = 0;
                for (const auto& [succ_id, prob] : succs_) {
                    if (succ_id == state_id) {
                        p_self += prob;
                        continue;
                    }

                    IDualData& succ_data = idual_data[succ_id];
                    if (succ_data.is_new() && !evaluate_state(
                                                  mdp,
                                                  heuristic,
                                                  mdp.get_state(succ_id),
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
            }
        }

        frontier.clear();

        update_hpom_constraints_frontier(
            mdp,
            idual_data,
            frontier_candidates,
            start_new_states);

        {
            TimerScope lp_scope(statistics_.lp_solver_timer);
            lp_solver_.solve();
            timer.throw_if_expired();
        }

        assert(lp_solver_.has_optimal_solution());
        objective_ = -lp_solver_.get_objective_value();
        std::vector<double> solution = lp_solver_.extract_solution();

        // Push frontier candidates and remove them
        std::erase_if(frontier_candidates, [&](StateID state_id) {
            for (const auto& [_, var_id] : idual_data[state_id].incoming) {
                if (solution[var_id] > fp_epsilon_) {
                    frontier.push_back(state_id);
                    return true;
                }
            }

            return false;
        });

        lp_solver_.clear_temporary_constraints();

        statistics_.iterations++;
        statistics_.open_states = frontier_candidates.size() + frontier.size();
    }

    statistics_.num_lp_vars = next_lp_var_;
    statistics_.num_lp_constraints = next_lp_constr_id_;

    return Interval(objective_, INFINITE_VALUE);
}

auto I2Dual::compute_policy(
    FDRMDP&,
    FDREvaluator&,
    const State&,
    ProgressReport,
    double) -> std::unique_ptr<PolicyType>
{
    abort();
}

bool I2Dual::evaluate_state(
    FDRMDP& mdp,
    FDREvaluator& heuristic,
    const State& state,
    IDualData& data)
{
    assert(data.is_new());

    const TerminationInfo term_info = mdp.get_termination_info(state);
    if (term_info.is_goal_state()) {
        data.set_terminal(0_vt);
        return true;
    }

    const value_t term_cost = term_info.get_cost();
    const value_t estimate = heuristic.evaluate(state);

    if (estimate == term_cost) {
        data.set_terminal(-estimate);
        return true;
    }

    data.open(next_lp_constr_id_++, -estimate);
    return false;
}

void I2Dual::prepare_lp()
{
    // setup empty LP
    lp::LinearProgram lp(
        lp::LPObjectiveSense::MAXIMIZE,
        named_vector::NamedVector<lp::LPVariable>(),
        named_vector::NamedVector<lp::LPConstraint>(),
        lp_solver_.get_infinity());

    prepare_hpom(lp);

    next_lp_var_ = lp.get_variables().size();
    next_lp_constr_id_ = lp.get_constraints().size();

    lp.get_constraints().emplace_back(-lp_solver_.get_infinity(), 1);
    lp_solver_.load_problem(lp);
}

void I2Dual::prepare_hpom(lp::LinearProgram& lp)
{
    if (!hpom_enabled_) {
        return;
    }

    TimerScope scope(statistics_.hpom_timer);
    occupation_measures::HPOMGenerator::generate_hpom_lp(
        task_proxy_,
        *task_cost_function_,
        lp,
        offset_);

    hpom_constraints_ = std::move(lp.get_constraints());

    statistics_.hpom_num_vars = lp.get_variables().size();
    statistics_.hpom_num_constraints = hpom_constraints_.size();
}

void I2Dual::update_hpom_constraints_expanded(
    FDRMDP& mdp,
    storage::PerStateStorage<IDualData>& data,
    const std::vector<StateID>& expanded)
{
    if (!hpom_enabled_ || !incremental_hpom_updates_) {
        return;
    }

    if (hpom_initialized_) {
        TimerScope scope(statistics_.hpom_timer);
        for (const StateID state_id : expanded) {
            remove_fringe_state_from_hpom(
                mdp.get_state(state_id),
                data[state_id],
                hpom_constraints_);
        }
    }

    hpom_initialized_ = true;
}

void I2Dual::update_hpom_constraints_frontier(
    FDRMDP& mdp,
    storage::PerStateStorage<IDualData>& data,
    const std::vector<StateID>& frontier,
    const unsigned start)
{
    if (!hpom_enabled_) {
        return;
    }

    TimerScope scope(statistics_.hpom_timer);

    size_t i = incremental_hpom_updates_ ? start : 0;

    for (; i < frontier.size(); ++i) {
        StateID state_id = frontier[i];
        State s = mdp.get_state(state_id);
        add_fringe_state_to_hpom(s, data[state_id], hpom_constraints_);
    }

    lp_solver_.add_temporary_constraints(hpom_constraints_);
}

void I2Dual::remove_fringe_state_from_hpom(
    const State& state,
    const IDualData& data,
    named_vector::NamedVector<lp::LPConstraint>& constraints) const
{
    for (VariableProxy var : task_proxy_.get_variables()) {
        const int val = state[var].get_value();
        lp::LPConstraint& c = constraints[offset_[var.get_id()] + val];
        for (const auto& om : data.incoming) {
            c.remove(om.second);
        }
    }
}

void I2Dual::add_fringe_state_to_hpom(
    const State& state,
    const IDualData& data,
    named_vector::NamedVector<lp::LPConstraint>& constraints) const
{
    for (VariableProxy var : task_proxy_.get_variables()) {
        const int val = state[var].get_value();
        lp::LPConstraint& c = constraints[offset_[var.get_id()] + val];
        for (const auto& om : data.incoming) {
            c.insert(om.second, om.first);
        }
    }
}

} // namespace probfd::algorithms::i2dual
