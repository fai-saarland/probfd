#include "probfd/heuristics/pdbs/engine_interfaces.h"

#include "probfd/heuristics/pdbs/maxprob_pattern_database.h"
#include "probfd/heuristics/pdbs/ssp_pattern_database.h"

#include "pdbs/pattern_database.h"

#include <limits>

namespace probfd {

using namespace heuristics::pdbs;

namespace engine_interfaces {

void StateSpace<StateRank, const AbstractOperator*>::
    generate_applicable_actions(
        StateID state_id,
        std::vector<const AbstractOperator*>& aops)
{
    match_tree_.get_applicable_operators(
        heuristics::pdbs::StateRank(state_id),
        aops);
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

template class IncrementalPPDBEvaluator<SSPPatternDatabase>;
template class IncrementalPPDBEvaluator<MaxProbPatternDatabase>;

template class MergeEvaluator<SSPPatternDatabase>;
template class MergeEvaluator<MaxProbPatternDatabase>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
