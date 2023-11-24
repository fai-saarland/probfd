#include "probfd/pdbs/projection_state_space.h"

#include "probfd/pdbs/state_ranking_function.h"
#include "probfd/pdbs/types.h"

#include "probfd/cost_function.h"
#include "probfd/task_proxy.h"

#include "downward/utils/countdown_timer.h"

namespace probfd {
namespace pdbs {

namespace {
// Footprint used for detecting duplicate operators.
struct ProgressionOperatorFootprint {
    value_t cost;
    long long int precondition_hash;
    Distribution<StateRank> successors;

    friend bool operator<(
        const ProgressionOperatorFootprint& a,
        const ProgressionOperatorFootprint& b)
    {
        return std::tie(a.cost, a.precondition_hash, a.successors) <
               std::tie(b.cost, b.precondition_hash, b.successors);
    }
};

struct OutcomeInfo {
    StateRank base_effect = StateRank(0);
    std::vector<int> missing_pres;

    friend bool operator<(const OutcomeInfo& a, const OutcomeInfo& b)
    {
        return std::tie(a.base_effect, a.missing_pres) <
               std::tie(b.base_effect, b.missing_pres);
    }

    friend bool operator==(const OutcomeInfo& a, const OutcomeInfo& b)
    {
        return std::tie(a.base_effect, a.missing_pres) ==
               std::tie(b.base_effect, b.missing_pres);
    }
};

} // namespace

ProjectionStateSpace::ProjectionStateSpace(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    const StateRankingFunction& ranking_function,
    bool operator_pruning,
    double max_time)
    : match_tree_(task_proxy.get_operators().size())
    , parent_cost_function(&task_cost_function)
    , goal_state_flags_(ranking_function.num_states(), false)
{
    utils::CountdownTimer timer(max_time);

    const Pattern& pattern = ranking_function.get_pattern();

    const VariablesProxy variables = task_proxy.get_variables();
    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();

    std::set<ProgressionOperatorFootprint> duplicates;

    std::vector<int> pdb_indices(variables.size(), -1);

    for (size_t i = 0; i < pattern.size(); ++i) {
        pdb_indices[pattern[i]] = i;
    }

    // Construct partial assignment ranking function for operator pruning
    std::vector<long long int> partial_multipliers;
    if (operator_pruning) {
        partial_multipliers.reserve(pattern.size());

        int p = 1;
        partial_multipliers.push_back(1);

        for (size_t i = 1; i != pattern.size(); ++i) {
            p *= ranking_function.get_domain_size(i - 1) + 1;
            partial_multipliers.push_back(p);
        }
    }

    // Generate the abstract operators for each probabilistic operator
    for (const ProbabilisticOperatorProxy& op : operators) {
        timer.throw_if_expired();

        const OperatorID operator_id(op.get_id());

        // Precondition partial state and partial state to enumerate
        // effect values not appearing in precondition
        std::vector<FactPair> local_precondition;

        for (const FactProxy& fact : op.get_preconditions()) {
            const auto [pre_var, pre_val] = fact.get_pair();
            const int pdb_index = pdb_indices[pre_var];

            if (pdb_index != -1) {
                local_precondition.emplace_back(pdb_index, pre_val);
            }
        }

        std::vector<FactPair> values;

        // Info about each probabilistic outcome
        Distribution<OutcomeInfo> outcomes;

        // Collect info about the outcomes
        for (const ProbabilisticOutcomeProxy& out : op.get_outcomes()) {
            OutcomeInfo info;

            for (ProbabilisticEffectProxy effect : out.get_effects()) {
                const auto [eff_var, eff_val] = effect.get_fact().get_pair();
                const int var = pdb_indices[eff_var];
                if (var == -1) continue;

                auto pre_it = std::ranges::lower_bound(
                    local_precondition,
                    var,
                    std::ranges::less(),
                    &FactPair::var);

                int val_change = eff_val;

                if (pre_it == local_precondition.end() || pre_it->var != var) {
                    values.emplace_back(var, 0);
                    info.missing_pres.push_back(var);
                } else {
                    val_change -= pre_it->value;
                }

                info.base_effect += ranking_function.rank_fact(var, val_change);
            }

            outcomes.add_probability(std::move(info), out.get_probability());
        }

        utils::sort_unique(values);

        // We enumerate all values for variables that are not part of
        // the precondition but in an effect. Depending on the value of the
        // variable, the value change caused by the abstract operator would
        // be different, hence we generate on operator for each state where
        // enabled.
        do {
            timer.throw_if_expired();

            // Generate the progression operator
            ProjectionOperator new_op(operator_id);

            for (const auto& [info, prob] : outcomes) {
                const auto& [base_effect, missing_pres] = info;

                StateRank offset = base_effect;

                auto it = values.begin();
                auto end = values.end();

                for (const int idx : missing_pres) {
                    it = std::find_if(it, end, [=](auto a) {
                        return a.var == idx;
                    });
                    assert(it != end);
                    offset -= ranking_function.rank_fact(idx, it++->value);
                }

                new_op.outcome_offsets.add_probability(offset, prob);
            }

            // Construct the precondition by merging the original
            // precondition partial state with the partial state for the
            // non-precondition effects of this iteration
            std::vector<FactPair> precondition;
            precondition.reserve(local_precondition.size() + values.size());

            std::ranges::merge(
                local_precondition,
                values,
                std::back_inserter(precondition));

            if (operator_pruning) {
                // Generate a rank for the precondition to check for
                // duplicates
                long long int pre_rank = 0;
                for (const auto& [var, val] : precondition) {
                    // Missing preconditions are -1, so we add 1 to adjust
                    // to the range [0, d + 1] where d is the domain size
                    pre_rank += partial_multipliers[var] * (val + 1);
                }

                const value_t cost =
                    task_cost_function.get_action_cost(operator_id);

                if (!duplicates.emplace(cost, pre_rank, new_op.outcome_offsets)
                         .second) {
                    continue;
                }
            }

            // Now add the progression operators to the match tree
            match_tree_.insert(
                variables,
                ranking_function,
                std::move(new_op),
                precondition);
        } while (ranking_function.next_partial_assignment(values));
    }

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
                base += ranking_function.rank_fact(v++, goal_fact.get_value());
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

    do {
        goal_state_flags_[base] = true;
    } while (ranking_function.next_rank(base, non_goal_vars));
}

StateID ProjectionStateSpace::get_state_id(StateRank state)
{
    return state;
}

StateRank ProjectionStateSpace::get_state(StateID id)
{
    return StateRank(id);
}

void ProjectionStateSpace::generate_applicable_actions(
    StateRank state,
    std::vector<const ProjectionOperator*>& aops)
{
    match_tree_.get_applicable_operators(state, aops);
}

void ProjectionStateSpace::generate_action_transitions(
    StateRank state,
    const ProjectionOperator* op,
    Distribution<StateID>& result)
{
    for (const auto& [offset, probability] : op->outcome_offsets) {
        result.add_probability(state + offset, probability);
    }
}

void ProjectionStateSpace::generate_all_transitions(
    StateRank state,
    std::vector<const ProjectionOperator*>& aops,
    std::vector<Distribution<StateID>>& result)
{
    generate_applicable_actions(state, aops);
    result.reserve(aops.size());
    for (const ProjectionOperator* op : aops) {
        generate_action_transitions(state, op, result.emplace_back());
    }
}

void ProjectionStateSpace::generate_all_transitions(
    StateRank state,
    std::vector<Transition>& transitions)
{
    match_tree_.generate_all_transitions(state, transitions, *this);
}

bool ProjectionStateSpace::is_goal(StateRank state) const
{
    return goal_state_flags_[state];
}

value_t ProjectionStateSpace::get_non_goal_termination_cost() const
{
    return parent_cost_function->get_non_goal_termination_cost();
}

value_t ProjectionStateSpace::get_action_cost(const ProjectionOperator* op)
{
    return parent_cost_function->get_action_cost(op->operator_id);
}

} // namespace pdbs
} // namespace probfd