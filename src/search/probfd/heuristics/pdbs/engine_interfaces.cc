#include "probfd/heuristics/pdbs/engine_interfaces.h"

#include "probfd/heuristics/pdbs/maxprob_pattern_database.h"
#include "probfd/heuristics/pdbs/projection_state_space.h"
#include "probfd/heuristics/pdbs/ssp_pattern_database.h"

#include "probfd/task_proxy.h"
#include "probfd/value_utils.h"

#include "pdbs/pattern_database.h"

#include <limits>
#include <ranges>
#include <vector>

namespace probfd {

using namespace heuristics::pdbs;

namespace engine_interfaces {

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

StateSpace<StateRank, const AbstractOperator*>::StateSpace(
    const ProbabilisticTaskProxy& task_proxy,
    const StateRankingFunction& ranking_function,
    bool operator_pruning)
    : match_tree_(task_proxy.get_operators().size())
{
    const Pattern& pattern = ranking_function.get_pattern();

    const VariablesProxy variables = task_proxy.get_variables();
    const ProbabilisticOperatorsProxy operators = task_proxy.get_operators();

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
        const int cost = op.get_cost();

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

            outcomes.add_unique(std::move(info), out.get_probability());
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
            AbstractOperator new_op(operator_id, cost);

            for (const auto& [info, prob] : outcomes) {
                const auto& [base_effect, missing_pres] = info;
                auto a =
                    ranking_function.from_values_partial(missing_pres, values);

                new_op.outcomes.add_unique(base_effect - a, prob);
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

                if (!duplicate_set.emplace(cost, pre_rank, new_op).second) {
                    continue;
                }
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

StateID StateSpace<StateRank, const AbstractOperator*>::get_state_id(
    StateRank state) const
{
    return state.id;
}

StateRank
StateSpace<StateRank, const AbstractOperator*>::get_state(StateID id) const
{
    return StateRank(id);
}

ActionID StateSpace<StateRank, const AbstractOperator*>::get_action_id(
    StateID,
    const AbstractOperator* op) const
{
    return match_tree_.get_operator_index(*op);
}

const AbstractOperator*
StateSpace<StateRank, const AbstractOperator*>::get_action(
    StateID,
    ActionID action_id) const
{
    return &match_tree_.get_index_operator(action_id.id);
}

void StateSpace<StateRank, const AbstractOperator*>::
    generate_applicable_actions(
        StateID state_id,
        std::vector<const AbstractOperator*>& aops)
{
    match_tree_.get_applicable_operators(StateRank(state_id), aops);
}

void StateSpace<StateRank, const AbstractOperator*>::
    generate_action_transitions(
        StateID state_id,
        const AbstractOperator* op,
        Distribution<StateID>& result)
{
    for (const auto& [item, probability] : op->outcomes) {
        result.add(state_id + item.id, probability);
    }
}

void StateSpace<StateRank, const AbstractOperator*>::generate_all_transitions(
    StateID state_id,
    std::vector<const AbstractOperator*>& aops,
    std::vector<Distribution<StateID>>& result)
{
    match_tree_.get_applicable_operators(
        heuristics::pdbs::StateRank(state_id),
        aops);
    result.reserve(aops.size());
    for (const AbstractOperator* op : aops) {
        auto& ops = result.emplace_back();
        for (const auto& [item, probability] : op->outcomes) {
            ops.add(state_id + item.id, probability);
        }
    }
}
} // namespace engine_interfaces

namespace heuristics {
namespace pdbs {

PDBEvaluator::PDBEvaluator(const ::pdbs::PatternDatabase& pdb)
    : pdb(pdb)
{
}

EvaluationResult PDBEvaluator::evaluate(const StateRank& state) const
{
    int deterministic_val = pdb.get_value_for_index(state.id);

    if (deterministic_val == std::numeric_limits<int>::max()) {
        return EvaluationResult(true, INFINITE_VALUE);
    }

    return EvaluationResult(false, static_cast<value_t>(deterministic_val));
}

DeadendPDBEvaluator::DeadendPDBEvaluator(const ::pdbs::PatternDatabase& pdb)
    : pdb(pdb)
{
}

EvaluationResult DeadendPDBEvaluator::evaluate(const StateRank& state) const
{
    bool dead =
        pdb.get_value_for_index(state.id) == std::numeric_limits<int>::max();

    return dead ? EvaluationResult(true, 1_vt) : EvaluationResult(false, 0_vt);
}

IncrementalPPDBEvaluatorBase::IncrementalPPDBEvaluatorBase(
    const StateRankingFunction* mapper,
    int add_var)
{
    const int idx = mapper->get_index(add_var);
    assert(idx != -1);

    this->domain_size = mapper->get_domain_size(idx);
    this->left_multiplier = mapper->get_multiplier(idx);
    this->right_multiplier =
        static_cast<unsigned int>(idx + 1) < mapper->num_vars()
            ? mapper->get_multiplier(idx + 1)
            : mapper->num_states();
}

StateRank IncrementalPPDBEvaluatorBase::to_parent_state(StateRank state) const
{
    int left = state.id % left_multiplier;
    int right = state.id - (state.id % right_multiplier);
    return StateRank(left + right / domain_size);
}

template <typename PDBType>
IncrementalPPDBEvaluator<PDBType>::IncrementalPPDBEvaluator(
    const PDBType& pdb,
    const StateRankingFunction* mapper,
    int add_var)
    : IncrementalPPDBEvaluatorBase(mapper, add_var)
    , pdb(pdb)
{
}

template <typename PDBType>
EvaluationResult
IncrementalPPDBEvaluator<PDBType>::evaluate(const StateRank& state) const
{
    return pdb.evaluate(to_parent_state(state));
}

template <typename PDBType>
MergeEvaluator<PDBType>::MergeEvaluator(
    const StateRankingFunction& mapper,
    const PDBType& left,
    const PDBType& right)
    : mapper(mapper)
    , left(left)
    , right(right)
{
}

template <typename PDBType>
EvaluationResult MergeEvaluator<PDBType>::evaluate(const StateRank& state) const
{
    StateRank lstate = mapper.convert(state, left.get_pattern());

    auto leval = left.evaluate(lstate);

    if (leval.is_unsolvable()) {
        return leval;
    }

    StateRank rstate = mapper.convert(state, right.get_pattern());

    auto reval = right.evaluate(rstate);

    if (reval.is_unsolvable()) {
        return reval;
    }

    return {false, std::min(leval.get_estimate(), reval.get_estimate())};
}

BaseAbstractCostFunction::BaseAbstractCostFunction(
    const ProjectionStateSpace& state_space,
    value_t value_in,
    value_t value_not_in)
    : state_space_(state_space)
    , value_in_(value_in)
    , value_not_in_(value_not_in)
{
}

TerminationInfo
BaseAbstractCostFunction::get_termination_info(const StateRank& state)
{
    const bool is_contained = state_space_.is_goal(state);
    return TerminationInfo(
        is_contained,
        is_contained ? value_in_ : value_not_in_);
}

value_t
ZeroCostAbstractCostFunction::get_action_cost(StateID, const AbstractOperator*)
{
    return 0;
}

value_t NormalCostAbstractCostFunction::get_action_cost(
    StateID,
    const AbstractOperator* op)
{
    return op->cost;
}

template class IncrementalPPDBEvaluator<SSPPatternDatabase>;
template class IncrementalPPDBEvaluator<MaxProbPatternDatabase>;

template class MergeEvaluator<SSPPatternDatabase>;
template class MergeEvaluator<MaxProbPatternDatabase>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
