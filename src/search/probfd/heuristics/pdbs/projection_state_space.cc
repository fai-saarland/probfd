#include "probfd/heuristics/pdbs/projection_state_space.h"

#include "probfd/task_proxy.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

ProjectionStateSpace::ProjectionStateSpace(
    const ProbabilisticTaskProxy& task_proxy,
    const StateRankingFunction& ranking_function,
    bool operator_pruning)
    : state_space(task_proxy, ranking_function, operator_pruning)
    , initial_state_(ranking_function.rank(task_proxy.get_initial_state()))
    , goal_state_flags_(ranking_function.num_states())
{
    const Pattern& pattern = ranking_function.get_pattern();

    const GoalsProxy task_goals = task_proxy.get_goals();

    std::vector<int> non_goal_vars;
    StateRank base(0);

    // Translate sparse goal into pdb index space
    // and collect non-goal variables aswell.
    const int num_goal_facts = task_goals.size();
    const int num_variables = pattern.size();

    for (int v = 0, w = 0; v != static_cast<int>(pattern.size());) {
        const int p_var = pattern[v];
        const FactProxy goal_fact = task_goals[w];
        const int g_var = goal_fact.get_variable().get_id();

        if (p_var < g_var) {
            non_goal_vars.push_back(v++);
        } else {
            if (p_var == g_var) {
                const int g_val = goal_fact.get_value();
                base.id += ranking_function.get_multiplier(v++) * g_val;
            }

            if (++w == num_goal_facts) {
                while (v < num_variables) {
                    non_goal_vars.push_back(v++);
                }
                break;
            }
        }
    }

    assert(non_goal_vars.size() != pattern.size()); // No goal no fun.

    auto goals = ranking_function.state_ranks(base, std::move(non_goal_vars));

    for (const auto& g : goals) {
        goal_state_flags_[g.id] = true;
    }
}

StateID ProjectionStateSpace::get_state_id(StateRank state) const
{
    return state_space.get_state_id(state);
}

StateRank ProjectionStateSpace::get_state(StateID id) const
{
    return state_space.get_state(id);
}

ActionID
ProjectionStateSpace::get_action_id(StateID id, const AbstractOperator* op)
    const
{
    return state_space.get_action_id(id, op);
}

const AbstractOperator*
ProjectionStateSpace::get_action(StateID id, ActionID action_id) const
{
    return state_space.get_action(id, action_id);
}

void ProjectionStateSpace::generate_applicable_actions(
    StateID state,
    std::vector<const AbstractOperator*>& aops)
{
    return state_space.generate_applicable_actions(state, aops);
}

void ProjectionStateSpace::generate_action_transitions(
    StateID state,
    const AbstractOperator* op,
    Distribution<StateID>& result)
{
    return state_space.generate_action_transitions(state, op, result);
}

void ProjectionStateSpace::generate_all_transitions(
    StateID state,
    std::vector<const AbstractOperator*>& aops,
    std::vector<Distribution<StateID>>& result)
{
    return state_space.generate_all_transitions(state, aops, result);
}

bool ProjectionStateSpace::is_goal(StateRank state) const
{
    return goal_state_flags_[state.id];
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd