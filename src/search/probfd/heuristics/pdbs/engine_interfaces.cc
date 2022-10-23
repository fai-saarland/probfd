#include "probfd/heuristics/pdbs/engine_interfaces.h"

#include "probfd/heuristics/pdbs/expcost_projection.h"
#include "probfd/heuristics/pdbs/maxprob_projection.h"
#include "probfd/heuristics/pdbs/qualitative_result_store.h"

#include "probfd/probabilistic_operator.h"

#include "pdbs/pattern_database.h"

#include "successor_generator.h"

#include <limits>

namespace probfd {

using namespace heuristics::pdbs;

namespace engine_interfaces {
unsigned StateIDMap<AbstractState>::size() const
{
    return seen.size();
}

StateIDMap<AbstractState>::visited_iterator
StateIDMap<AbstractState>::visited_begin() const
{
    return seen.cbegin();
}

StateIDMap<AbstractState>::visited_iterator
StateIDMap<AbstractState>::visited_end() const
{
    return seen.cend();
}

StateIDMap<AbstractState>::visited_range
StateIDMap<AbstractState>::visited() const
{
    return visited_range(visited_begin(), visited_end());
}

StateID StateIDMap<AbstractState>::get_state_id(const AbstractState& state)
{
    seen.insert(state.id);
    return StateID(state.id);
}

AbstractState StateIDMap<AbstractState>::get_state(const StateID& id)
{
    return AbstractState(id);
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
    StateIDMap<AbstractState>& id_map,
    std::shared_ptr<AbstractStateMapper> state_mapper,
    std::shared_ptr<
        successor_generator::SuccessorGenerator<const AbstractOperator*>>
        aops_gen)
    : id_map_(id_map)
    , state_mapper_(state_mapper)
    , values_(state_mapper->get_pattern().size(), 0)
    , aops_gen_(aops_gen)
{
}

void TransitionGenerator<const AbstractOperator*>::operator()(
    const StateID& sid,
    std::vector<const AbstractOperator*>& aops)
{
    AbstractState abstract_state = id_map_.get_state(sid);
    state_mapper_->to_values(abstract_state, values_);
    aops_gen_->generate_applicable_ops(values_, aops);
}

void TransitionGenerator<const AbstractOperator*>::operator()(
    const StateID& state,
    const AbstractOperator* op,
    Distribution<StateID>& result)
{
    AbstractState abstract_state = id_map_.get_state(state);
    for (auto it = op->outcomes.begin(); it != op->outcomes.end(); it++) {
        const AbstractState succ = abstract_state + it->element;
        result.add(id_map_.get_state_id(succ), it->probability);
    }
}

void TransitionGenerator<const AbstractOperator*>::operator()(
    const StateID& state,
    std::vector<const AbstractOperator*>& aops,
    std::vector<Distribution<StateID>>& result)
{
    AbstractState abstract_state = id_map_.get_state(state);
    state_mapper_->to_values(abstract_state, values_);
    aops_gen_->generate_applicable_ops(values_, aops);
    result.resize(aops.size());
    for (int i = aops.size() - 1; i >= 0; --i) {
        const AbstractOperator* op = aops[i];
        for (auto it = op->outcomes.begin(); it != op->outcomes.end(); it++) {
            const AbstractState succ = abstract_state + it->element;
            result[i].add(id_map_.get_state_id(succ), it->probability);
        }
    }
}
} // namespace engine_interfaces

namespace heuristics {
namespace pdbs {

AbstractStateDeadendStoreEvaluator::AbstractStateDeadendStoreEvaluator(
    const QualitativeResultStore* states,
    value_type::value_t value_in,
    value_type::value_t value_not_in)
    : states_(states)
    , value_in_(value_in)
    , value_not_in_(value_not_in)
{
}

EvaluationResult
AbstractStateDeadendStoreEvaluator::evaluate(const AbstractState& state) const
{
    const bool is_contained = states_->get(state);
    return EvaluationResult(
        is_contained,
        is_contained ? value_in_ : value_not_in_);
}

AbstractStateInStoreRewardFunction::AbstractStateInStoreRewardFunction(
    const QualitativeResultStore* states,
    value_type::value_t value_in,
    value_type::value_t value_not_in)
    : states_(states)
    , value_in_(value_in)
    , value_not_in_(value_not_in)
{
}

EvaluationResult
AbstractStateInStoreRewardFunction::evaluate(const AbstractState& state)
{
    const bool is_contained = states_->get(state);
    return EvaluationResult(
        is_contained,
        is_contained ? value_in_ : value_not_in_);
}

PDBEvaluator::PDBEvaluator(const ::pdbs::PatternDatabase& pdb)
    : pdb(pdb)
{
}

EvaluationResult PDBEvaluator::evaluate(const AbstractState& state) const
{
    int deterministic_val = pdb.get_value_for_index(state.id);

    return EvaluationResult(
        deterministic_val == std::numeric_limits<int>::max(),
        static_cast<value_type::value_t>(deterministic_val));
}

DeadendPDBEvaluator::DeadendPDBEvaluator(const ::pdbs::PatternDatabase& pdb)
    : pdb(pdb)
{
}

EvaluationResult DeadendPDBEvaluator::evaluate(const AbstractState& state) const
{
    bool dead =
        pdb.get_value_for_index(state.id) == std::numeric_limits<int>::max();

    return dead ? EvaluationResult(true, value_type::zero)
                : EvaluationResult(false, value_type::one);
}

IncrementalPPDBEvaluatorBase::IncrementalPPDBEvaluatorBase(
    const AbstractStateMapper* mapper,
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

AbstractState
IncrementalPPDBEvaluatorBase::to_parent_state(AbstractState state) const
{
    int left = state.id % left_multiplier;
    int right = state.id - (state.id % right_multiplier);
    return AbstractState(left + right / domain_size);
}

template <typename PDBType>
IncrementalPPDBEvaluator<PDBType>::IncrementalPPDBEvaluator(
    const PDBType& pdb,
    const AbstractStateMapper* mapper,
    int add_var)
    : IncrementalPPDBEvaluatorBase(mapper, add_var)
    , pdb(pdb)
{
}

template <typename PDBType>
EvaluationResult
IncrementalPPDBEvaluator<PDBType>::evaluate(const AbstractState& state) const
{
    return pdb.evaluate(to_parent_state(state));
}

template class IncrementalPPDBEvaluator<ExpCostProjection>;
template class IncrementalPPDBEvaluator<MaxProbProjection>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
