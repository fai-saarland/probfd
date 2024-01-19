#ifndef GUARD_INCLUDE_PROBFD_ALGORITHMS_IDUAL_H
#error "This file should only be included from idual.h"
#endif

#include "probfd/algorithms/types.h"
#include "probfd/algorithms/utils.h"

#include "probfd/distribution.h"
#include "probfd/evaluator.h"

#include "downward/utils/countdown_timer.h"

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
    MDP& mdp,
    Evaluator& heuristic,
    param_type<State> initial_state,
    ProgressReport progress,
    double max_time)
{
    using namespace std::views;

    utils::CountdownTimer timer(max_time);

    const double eps = g_epsilon;
    const double inf = lp_solver_.get_infinity();

    StateID prev_state = StateID::UNDEFINED;
    unsigned next_var_id = 0;
    unsigned next_constraint_id = 0;
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

        if (estimate == term_cost) {
            return Interval(estimate);
        }

        named_vector::NamedVector<lp::LPVariable> vars;
        named_vector::NamedVector<lp::LPConstraint> constraints;

        vars.emplace_back(-estimate, inf, 1.0);
        constraints.emplace_back(-inf, inf);

        lp_solver_.load_problem(lp::LinearProgram(
            lp::LPObjectiveSense::MINIMIZE,
            std::move(vars),
            std::move(constraints),
            inf));
        prev_state = mdp.get_state_id(initial_state);
        PerStateInfo& info = state_infos_[prev_state];
        info.idx = next_var_id;
        info.status = PerStateInfo::CLOSED;
        frontier.push_back(prev_state);
        ++next_var_id;
        ++next_constraint_id;
    }

    std::vector<Action> aops;
    std::vector<Distribution<StateID>> transitions;
    std::vector<double> lp_sol;

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

            const unsigned var_id = state_infos_[state_id].idx;
            assert(state_infos_[state_id].status == PerStateInfo::CLOSED);

            lp_solver_.set_variable_lower_bound(var_id, -t_cost);

            if (term_info.is_goal_state()) {
                continue;
            }

            ClearGuard _(aops, transitions);
            mdp.generate_all_transitions(state, aops, transitions);

            for (const auto [action, transition] : zip(aops, transitions)) {
                if (transition.is_dirac(state_id)) continue;

                lp::LPConstraint c(-inf, inf);

                double base_val = -mdp.get_action_cost(action);
                StateID next_prev_state = prev_state;
                double w = 1.0;

                for (const auto& [succ_id, prob] : transition) {
                    if (succ_id == state_id) {
                        w -= prob;
                        continue;
                    }

                    PerStateInfo& succ_info = state_infos_[succ_id];

                    if (succ_id > prev_state) {
                        assert(state_infos_[succ_id].idx == (unsigned)-1);

                        State succ_state = mdp.get_state(succ_id);
                        const auto term = mdp.get_termination_info(succ_state);
                        const value_t term_cost = term.get_cost();
                        const value_t estimate = heuristic.evaluate(succ_state);

                        const auto value = -estimate;

                        if (term_cost == estimate) {
                            succ_info.status = PerStateInfo::TERMINAL;
                            succ_info.idx = terminals_.get_id(value);
                            base_val += prob * value;
                        } else {
                            lp_solver_.add_variable(
                                lp::LPVariable(value, inf, 0.0),
                                std::vector<int>(),
                                std::vector<double>());
                            succ_info.idx = next_var_id;
                            succ_info.status = PerStateInfo::OPEN;
                            c.insert(next_var_id, -prob);
                            open_states[succ_id].incoming.push_back(
                                next_constraint_id);
                            ++next_var_id;
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
                            c.insert(succ_info.idx, -prob);
                            break;
                        default:
                            base_val += prob * terminals_[succ_info.idx];
                            break;
                        }
                    }
                }

                prev_state = next_prev_state;

                assert(w > 0_vt);
                c.insert(var_id, w);
                c.set_lower_bound(base_val);
                lp_solver_.add_constraint(c);
                ++next_constraint_id;
            }
        }

        frontier.clear();

        lp_solver_.solve();

        timer.throw_if_expired();

        assert(lp_solver_.has_optimal_solution());
        lp_sol = lp_solver_.extract_dual_solution();
        objective = -lp_solver_.get_objective_value();

        open_states.erase_if([&](const auto& pair) {
            double sum = 0;

            for (auto& r : pair.second.incoming) {
                sum += lp_sol[r];
            }

            if (sum > eps) {
                state_infos_[pair.first.id].status = PerStateInfo::CLOSED;
                frontier.emplace_back(pair.first);
                return true;
            }

            return false;
        });

        progress.print();
    } while (!frontier.empty());

    assert(!lp_sol.empty());

    statistics_.lp_variables = next_var_id;
    statistics_.lp_constraints = next_constraint_id;
    statistics_.open = open_states.size();

    return Interval(objective, INFINITE_VALUE);
}

} // namespace probfd::algorithms::idual
