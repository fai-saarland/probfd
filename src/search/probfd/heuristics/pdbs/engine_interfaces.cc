#include "probfd/heuristics/pdbs/engine_interfaces.h"

#include "probfd/heuristics/pdbs/expcost_projection.h"
#include "probfd/heuristics/pdbs/maxprob_projection.h"

#include "pdbs/pattern_database.h"

#include <limits>

namespace probfd {

using namespace heuristics::pdbs;

namespace engine_interfaces {
unsigned StateIDMap<StateRank>::size() const
{
    return seen.size();
}

StateIDMap<StateRank>::visited_iterator
StateIDMap<StateRank>::visited_begin() const
{
    return seen.cbegin();
}

StateIDMap<StateRank>::visited_iterator
StateIDMap<StateRank>::visited_end() const
{
    return seen.cend();
}

StateIDMap<StateRank>::visited_range StateIDMap<StateRank>::visited() const
{
    return visited_range(visited_begin(), visited_end());
}

StateID StateIDMap<StateRank>::get_state_id(const StateRank& state)
{
    seen.insert(state.id);
    return StateID(state.id);
}

StateRank StateIDMap<StateRank>::get_state(const StateID& id)
{
    seen.insert(id);
    return StateRank(id);
}

ActionIDMap<const AbstractOperator*>::ActionIDMap(
    const std::vector<AbstractOperator>& ops)
    : ops_(ops)
{
}

ActionID ActionIDMap<const AbstractOperator*>::get_action_id(
    const StateID&,
    const AbstractOperator* op) const
{
    return op - (&ops_[0]);
}

const AbstractOperator* ActionIDMap<const AbstractOperator*>::get_action(
    const StateID&,
    const ActionID& idx) const
{
    return (&ops_[0]) + idx;
}

TransitionGenerator<const AbstractOperator*>::TransitionGenerator(
    StateIDMap<StateRank>& id_map,
    const MatchTree& aops_gen)
    : id_map_(id_map)
    , aops_gen_(aops_gen)
{
}

void TransitionGenerator<const AbstractOperator*>::generate_applicable_actions(
    const StateID& sid,
    std::vector<const AbstractOperator*>& aops)
{
    StateRank abstract_state = id_map_.get_state(sid);
    aops_gen_.get_applicable_operators(abstract_state, aops);
}

void TransitionGenerator<const AbstractOperator*>::generate_action_transitions(
    const StateID& state,
    const AbstractOperator* op,
    Distribution<StateID>& result)
{
    StateRank abstract_state = id_map_.get_state(state);
    for (auto it = op->outcomes.begin(); it != op->outcomes.end(); it++) {
        const StateRank succ = abstract_state + it->element;
        result.add(id_map_.get_state_id(succ), it->probability);
    }
}

void TransitionGenerator<const AbstractOperator*>::generate_all_transitions(
    const StateID& state,
    std::vector<const AbstractOperator*>& aops,
    std::vector<Distribution<StateID>>& result)
{
    StateRank abstract_state = id_map_.get_state(state);
    aops_gen_.get_applicable_operators(abstract_state, aops);
    result.resize(aops.size());
    for (int i = aops.size() - 1; i >= 0; --i) {
        const AbstractOperator* op = aops[i];
        for (auto it = op->outcomes.begin(); it != op->outcomes.end(); it++) {
            const StateRank succ = abstract_state + it->element;
            result[i].add(id_map_.get_state_id(succ), it->probability);
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
        return EvaluationResult(true, -INFINITE_VALUE);
    }

    return EvaluationResult(false, -static_cast<value_t>(deterministic_val));
}

DeadendPDBEvaluator::DeadendPDBEvaluator(const ::pdbs::PatternDatabase& pdb)
    : pdb(pdb)
{
}

EvaluationResult DeadendPDBEvaluator::evaluate(const StateRank& state) const
{
    bool dead =
        pdb.get_value_for_index(state.id) == std::numeric_limits<int>::max();

    return dead ? EvaluationResult(true, 0_vt) : EvaluationResult(false, 1_vt);
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

template class IncrementalPPDBEvaluator<ExpCostProjection>;
template class IncrementalPPDBEvaluator<MaxProbProjection>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
