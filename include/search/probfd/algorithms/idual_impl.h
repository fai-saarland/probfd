#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_IDUAL_H
#error "This file should only be included from idual.h"
#endif

#include "probfd/algorithms/types.h"
#include "probfd/algorithms/utils.h"

#include "probfd/utils/not_implemented.h"

#include "probfd/distribution.h"
#include "probfd/evaluator.h"
#include "probfd/transition.h"

#include "downward/utils/countdown_timer.h"
#include "probfd/policies/map_policy.h"

#include <deque>

namespace probfd::algorithms::idual {

inline void Statistics::print(std::ostream& out) const
{
    out << "  Iterations: " << iterations << std::endl;
    out << "  Expansions: " << expansions << std::endl;
    out << "  Open states: " << open << std::endl;
    out << "  LP Variables: " << lp_variables << std::endl;
    out << "  LP Constraints: " << lp_constraints << std::endl;
}

inline unsigned ValueGroup::get_id(value_t val)
{
    values_.push_back(val);
    auto it = indices_.insert(values_.size() - 1);
    if (!it.second) {
        values_.pop_back();
    }

    return *it.first;
}

template <typename State, typename Action>
IDual<State, Action>::IDual(lp::LPSolverType solver_type)
    : lp_solver_(solver_type)
{
}

template <typename State, typename Action>
Interval IDual<State, Action>::solve(
    MDPType& mdp,
    EvaluatorType& heuristic,
    param_type<State> initial_state,
    ProgressReport progress,
    double max_time)
{
    std::vector<double> primal_solution;
    std::vector<double> dual_solution;
    return solve(
        mdp,
        heuristic,
        initial_state,
        progress,
        max_time,
        primal_solution,
        dual_solution);
}

template <typename State, typename Action>
auto IDual<State, Action>::compute_policy(
    MDPType& mdp,
    EvaluatorType& heuristic,
    param_type<State> initial_state,
    ProgressReport progress,
    double max_time) -> std::unique_ptr<PolicyType>
{
    std::vector<double> primal_solution;
    std::vector<double> dual_solution;
    solve(
        mdp,
        heuristic,
        initial_state,
        progress,
        max_time,
        primal_solution,
        dual_solution);

    // The entries in the solution with a positive value represent transitions
    // which have positive occupation measure under the computed policy, i.e.,
    // which are selected by the policy with positive probability.
    // The policy can however be stochastic, i.e., select multiple transitions,
    // if traps exist.
    // To transform the policy to a deterministic optimal one, we first
    // exhaustively explore the policy and build the inverse edge relation.
    // Then we exhaustively traverse the policy backwards from the terminal
    // states with duplicate checking. Upon generating a predecessor via an
    // inverse edge, we select the corresponding action for the policy before
    // marking it as visited and adding it to the exploration queue.

    struct Edge {
        StateID predecessor;
        Action action;
    };

    const StateID initial_state_id = mdp.get_state_id(initial_state);

    // Explore the policy graph and build the inverse edge relation.
    storage::PerStateStorage<std::vector<Edge>> predecessor_edges;

    std::deque<StateID> queue{initial_state_id};
    std::unordered_set<StateID> visited{initial_state_id};

    std::deque<StateID> back_queue;

    do {
        const StateID state_id = queue.front();
        queue.pop_front();

        std::vector<Action> aops;
        const State state = mdp.get_state(state_id);
        mdp.generate_applicable_actions(state, aops);

        int constraint_index = state_infos_[state_id].constraints_idx;

        size_t actions = 0;
        int i = 0;
        for (const Action& action : aops) {
            if (dual_solution[constraint_index + i] > g_epsilon) {
                Distribution<StateID> distribution;
                mdp.generate_action_transitions(state, action, distribution);

                for (const StateID succ_id : distribution.support()) {
                    predecessor_edges[succ_id].emplace_back(state_id, action);
                    if (!visited.insert(succ_id).second) continue;
                    queue.push_back(succ_id);
                }

                ++actions;
            }
            ++i;
        }

        if (actions == 0) {
            back_queue.push_back(state_id);
        }
    } while (!queue.empty());

    // Now do the backwards exploration and extract a deterministic policy.
    visited.clear();

    visited.insert(back_queue.begin(), back_queue.end());

    auto policy = std::make_unique<policies::MapPolicy<State, Action>>(&mdp);

    while (!back_queue.empty()) {
        const StateID state_id = back_queue.front();
        back_queue.pop_front();

        for (auto& edge : predecessor_edges[state_id]) {
            if (!visited.insert(edge.predecessor).second) continue;
            policy->emplace_decision(
                edge.predecessor,
                edge.action,
                Interval(
                    primal_solution[state_infos_[edge.predecessor].var_idx]));
            back_queue.push_back(edge.predecessor);
        }
    }

    return policy;
}

template <typename State, typename Action>
Interval IDual<State, Action>::solve(
    MDPType& mdp,
    EvaluatorType& heuristic,
    param_type<State> initial_state,
    ProgressReport progress,
    double max_time,
    std::vector<double>& primal_solution,
    std::vector<double>& dual_solution)
{
    using namespace std::views;

    utils::CountdownTimer timer(max_time);

    const double inf = lp_solver_.get_infinity();

    StateID prev_state = StateID::UNDEFINED;
    std::vector<StateID> frontier;
    storage::StateHashMap<FrontierStateInfo> open_states;

    {
        // initialize lp
        const TerminationInfo term = mdp.get_termination_info(initial_state);

        if (term.is_goal_state()) {
            return Interval(0_vt);
        }

        const value_t term_cost = term.get_cost();
        const value_t estimate = heuristic.evaluate(initial_state);

        assert(estimate <= term_cost);

        if (estimate == term_cost) {
            return Interval(estimate);
        }

        named_vector::NamedVector<lp::LPVariable> vars;
        named_vector::NamedVector<lp::LPConstraint> constraints;

        vars.emplace_back(-inf, estimate, 1.0);

        lp_solver_.load_problem(lp::LinearProgram(
            lp::LPObjectiveSense::MAXIMIZE,
            std::move(vars),
            std::move(constraints),
            inf));
        prev_state = mdp.get_state_id(initial_state);
        PerStateInfo& info = state_infos_[prev_state];
        info.var_idx = 0;
        info.status = PerStateInfo::CLOSED;
        frontier.push_back(prev_state);
    }

    std::vector<Transition<Action>> transitions;

    value_t objective = 0_vt;

    progress.register_bound("v", [&] {
        return Interval(objective, INFINITE_VALUE);
    });

    progress.register_print([&](std::ostream& out) {
        out << "iteration=" << statistics_.iterations;
    });

    do {
        ++statistics_.iterations;
        statistics_.expansions += frontier.size();

        for (const StateID state_id : frontier) {
            timer.throw_if_expired();

            const State state = mdp.get_state(state_id);
            const TerminationInfo term_info = mdp.get_termination_info(state);
            const auto t_cost = term_info.get_cost();

            auto& info = state_infos_[state_id];
            const unsigned var_id = info.var_idx;
            assert(info.status == PerStateInfo::CLOSED);
            info.constraints_idx = lp_solver_.get_num_constraints();

            lp_solver_.set_variable_upper_bound(var_id, t_cost);

            if (term_info.is_goal_state()) {
                continue;
            }

            ClearGuard _(transitions);
            mdp.generate_all_transitions(state, transitions);

            for (const auto [action, transition] : transitions) {
                if (transition.is_dirac(state_id)) continue;

                int next_constraint_id = lp_solver_.get_num_constraints();
                lp::LPConstraint c(-inf, inf);

                double base_val = mdp.get_action_cost(action);
                StateID next_prev_state = prev_state;
                double w = 1.0;

                for (const auto& [succ_id, prob] : transition) {
                    if (succ_id == state_id) {
                        w -= prob;
                        continue;
                    }

                    PerStateInfo& succ_info = state_infos_[succ_id];

                    if (succ_id > prev_state) {
                        assert(
                            state_infos_[succ_id].var_idx ==
                            std::numeric_limits<unsigned>::max());

                        const State succ_state = mdp.get_state(succ_id);
                        const auto term = mdp.get_termination_info(succ_state);
                        const value_t term_cost = term.get_cost();
                        const value_t estimate = heuristic.evaluate(succ_state);

                        if (term_cost == estimate) {
                            succ_info.status = PerStateInfo::TERMINAL;
                            succ_info.var_idx = terminals_.get_id(estimate);
                            base_val += prob * estimate;
                        } else {
                            int next_var_id = lp_solver_.get_num_variables();
                            lp_solver_.add_variable(
                                lp::LPVariable(-inf, estimate, 0.0),
                                std::vector<int>(),
                                std::vector<double>());
                            succ_info.status = PerStateInfo::OPEN;
                            succ_info.var_idx = next_var_id;
                            c.insert(next_var_id, -prob);
                            open_states[succ_id].incoming.push_back(
                                next_constraint_id);
                        }

                        if (succ_id > next_prev_state) {
                            next_prev_state = succ_id;
                        }
                    } else {
                        assert(succ_info.status != PerStateInfo::NEW);

                        switch (succ_info.status) {
                        case PerStateInfo::OPEN:
                            open_states[succ_id].incoming.push_back(
                                next_constraint_id);
                            [[fallthrough]];
                        case PerStateInfo::CLOSED:
                            c.insert(succ_info.var_idx, -prob);
                            break;
                        default:
                            base_val += prob * terminals_[succ_info.var_idx];
                            break;
                        }
                    }
                }

                prev_state = next_prev_state;

                assert(w > 0_vt);
                c.insert(var_id, w);
                c.set_upper_bound(base_val);
                lp_solver_.add_constraint(c);
            }
        }

        frontier.clear();

        lp_solver_.solve();

        timer.throw_if_expired();

        assert(lp_solver_.has_optimal_solution());
        primal_solution = lp_solver_.extract_solution();
        dual_solution = lp_solver_.extract_dual_solution();
        objective = lp_solver_.get_objective_value();

        open_states.erase_if([&](const auto& pair) {
            for (auto& r : pair.second.incoming) {
                if (dual_solution[r] > g_epsilon) { // inflow > 0
                    state_infos_[pair.first.id].status = PerStateInfo::CLOSED;
                    frontier.emplace_back(pair.first);
                    return true;
                }
            }

            return false;
        });

        progress.print();
    } while (!frontier.empty());

    assert(!dual_solution.empty());

    statistics_.lp_variables = lp_solver_.get_num_variables();
    statistics_.lp_constraints = lp_solver_.get_num_constraints();
    statistics_.open = open_states.size();

    return Interval(objective, INFINITE_VALUE);
}

} // namespace probfd::algorithms::idual
