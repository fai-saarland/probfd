#include "probfd/heuristics/pdbs/projection_state_space.h"

#include "probfd/task_proxy.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

namespace {
// Footprint used for detecting duplicate operators.
struct ProgressionOperatorFootprint {
    value_t cost;
    long long int precondition_hash;
    std::vector<ItemProbabilityPair<StateRank>> successors;

    ProgressionOperatorFootprint(
        value_t cost,
        long long int precondition_hash,
        const AbstractOperator& op)
        : cost(cost)
        , precondition_hash(precondition_hash)
        , successors(op.outcomes.begin(), op.outcomes.end())
    {
        std::ranges::sort(successors);
    }

    friend auto operator<=>(
        const ProgressionOperatorFootprint& a,
        const ProgressionOperatorFootprint& b) = default;
};

struct OutcomeInfo {
    StateRank base_effect = StateRank(0);
    std::vector<int> missing_pres;

    friend auto
    operator<=>(const OutcomeInfo& a, const OutcomeInfo& b) = default;
};

} // namespace

ProjectionStateSpace::ProjectionStateSpace(
    const ProbabilisticTaskProxy& task_proxy,
    const StateRankingFunction& ranking_function,
    bool operator_pruning)
    : match_tree_(task_proxy.get_operators().size())
{
    const Pattern& pattern = ranking_function.get_pattern();

    const VariablesProxy variables = task_proxy.get_variables();
    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();
    const GoalsProxy task_goals = task_proxy.get_goals();

    std::set<ProgressionOperatorFootprint> duplicate_set;

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
        const int operator_id = op.get_id();

        // Precondition partial state and partial state to enumerate
        // effect values not appearing in precondition
        std::vector<FactPair> local_precondition;

        for (FactProxy fact : op.get_preconditions()) {
            const int pre_var = fact.get_variable().get_id();
            const int pdb_index = pdb_indices[pre_var];

            if (pdb_index != -1) {
                local_precondition.emplace_back(pdb_index, fact.get_value());
            }
        }

        std::vector<FactPair> vars_eff_not_pre;

        // Info about each probabilistic outcome
        Distribution<OutcomeInfo> outcomes;

        // Collect info about the outcomes
        for (const ProbabilisticOutcomeProxy& out : op.get_outcomes()) {
            OutcomeInfo info;

            std::vector<FactPair> local_effect;

            for (ProbabilisticEffectProxy effect : out.get_effects()) {
                FactProxy fact = effect.get_fact();
                const int eff_var = fact.get_variable().get_id();
                const int pdb_index = pdb_indices[eff_var];
                if (pdb_index != -1) {
                    local_effect.emplace_back(pdb_index, fact.get_value());
                }
            }

            for (const auto& [var, val] : local_effect) {
                auto pre_it = std::ranges::lower_bound(
                    local_precondition,
                    var,
                    std::ranges::less(),
                    &FactPair::var);

                int val_change = val;

                if (pre_it == local_precondition.end() || pre_it->var != var) {
                    vars_eff_not_pre.emplace_back(var, 0);
                    info.missing_pres.push_back(var);
                } else {
                    val_change -= pre_it->value;
                }

                info.base_effect += ranking_function.from_fact(var, val_change);
            }

            outcomes.add_probability(std::move(info), out.get_probability());
        }

        utils::sort_unique(vars_eff_not_pre);

        // We enumerate all values for variables that are not part of
        // the precondition but in an effect. Depending on the value of the
        // variable, the value change caused by the abstract operator would
        // be different, hence we generate on operator for each state where
        // enabled.
        auto ran =
            ranking_function.partial_assignments(std::move(vars_eff_not_pre));

        for (const std::vector<FactPair>& values : ran) {
            // Generate the progression operator
            AbstractOperator new_op(operator_id);

            for (const auto& [info, prob] : outcomes) {
                const auto& [base_effect, missing_pres] = info;
                auto a =
                    ranking_function.from_values_partial(missing_pres, values);

                new_op.outcomes.add_probability(base_effect - a, prob);
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

                // if (!duplicate_set.emplace(cost, pre_rank, new_op).second) {
                //     continue;
                // }
            }

            // Now add the progression operators to the match tree
            match_tree_.insert(
                variables,
                ranking_function,
                std::move(new_op),
                precondition);
        }
    }
}

StateID ProjectionStateSpace::get_state_id(StateRank state)
{
    return state.id;
}

StateRank ProjectionStateSpace::get_state(StateID id)
{
    return StateRank(id);
}

ActionID
ProjectionStateSpace::get_action_id(StateID, const AbstractOperator* op)
{
    return match_tree_.get_operator_index(*op);
}

const AbstractOperator*
ProjectionStateSpace::get_action(StateID, ActionID action_id)
{
    return &match_tree_.get_index_operator(action_id.id);
}

void ProjectionStateSpace::generate_applicable_actions(
    StateID state,
    std::vector<const AbstractOperator*>& aops)
{
    match_tree_.get_applicable_operators(StateRank(state.id), aops);
}

void ProjectionStateSpace::generate_action_transitions(
    StateID state,
    const AbstractOperator* op,
    Distribution<StateID>& result)
{
    for (const auto& [item, probability] : op->outcomes) {
        result.add_probability(state.id + item.id, probability);
    }
}

void ProjectionStateSpace::generate_all_transitions(
    StateID state,
    std::vector<const AbstractOperator*>& aops,
    std::vector<Distribution<StateID>>& result)
{
    generate_applicable_actions(state, aops);
    result.reserve(aops.size());
    for (const AbstractOperator* op : aops) {
        generate_action_transitions(state, op, result.emplace_back());
    }
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd