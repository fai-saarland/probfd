#include "engine_interfaces.h"

#include "../../../successor_generator.h"
#include "../../probabilistic_operator.h"
#include "qualitative_result_store.h"

namespace probabilistic {

StateIDMap<pdbs::AbstractState>::StateIDMap()
    : next_id_(0)
    , id_map_()
    , inverse_map_()
{
}

unsigned
StateIDMap<pdbs::AbstractState>::size() const
{
    return id_map_.size();
}

StateIDMap<pdbs::AbstractState>::indirection_iterator
StateIDMap<pdbs::AbstractState>::indirection_begin() const
{
    return id_map_.begin();
}

StateIDMap<pdbs::AbstractState>::indirection_iterator
StateIDMap<pdbs::AbstractState>::indirection_end() const
{
    return id_map_.end();
}

StateID
StateIDMap<pdbs::AbstractState>::get_state_id(const pdbs::AbstractState& state)
{
    auto it = id_map_.emplace(state.id, next_id_);
    if (it.second) {
        ++next_id_;
        inverse_map_.emplace_back(state.id);
    }
    return StateID(it.first->second);
}

pdbs::AbstractState
StateIDMap<pdbs::AbstractState>::get_state(const StateID& id)
{
    assert(id < inverse_map_.size());
    return pdbs::AbstractState(inverse_map_[id]);
}

ActionIDMap<const pdbs::AbstractOperator*>::ActionIDMap(
    const std::vector<pdbs::AbstractOperator>& ops)
    : ops_(ops)
{
}

ActionID
ActionIDMap<const pdbs::AbstractOperator*>::get_action_id(
    const StateID&,
    const pdbs::AbstractOperator* op) const
{
    return op - (&ops_[0]);
}

const pdbs::AbstractOperator*
ActionIDMap<const pdbs::AbstractOperator*>::get_action(
    const StateID&,
    const ActionID& idx) const
{
    return (&ops_[0]) + idx;
}

ApplicableActionsGenerator<const pdbs::AbstractOperator*>::
    ApplicableActionsGenerator(
        StateIDMap<pdbs::AbstractState>& id_map,
        std::shared_ptr<pdbs::AbstractStateMapper> state_mapper,
        std::shared_ptr<successor_generator::SuccessorGenerator<
            const pdbs::AbstractOperator*>> aops_gen)
    : id_map_(id_map)
    , state_mapper_(state_mapper)
    , values_(state_mapper->get_variables().size(), 0)
    , aops_gen_(aops_gen)
{
}

void
ApplicableActionsGenerator<const pdbs::AbstractOperator*>::operator()(
    const StateID& sid,
    std::vector<const pdbs::AbstractOperator*>& aops)
{
    pdbs::AbstractState abstract_state = id_map_.get_state(sid);
    state_mapper_->to_values(abstract_state, values_);
    aops_gen_->generate_applicable_ops(values_, aops);
}

TransitionGenerator<const pdbs::AbstractOperator*>::TransitionGenerator(
    StateIDMap<pdbs::AbstractState>& id_map,
    std::shared_ptr<pdbs::AbstractStateMapper> state_mapper,
    std::shared_ptr<
        successor_generator::SuccessorGenerator<const pdbs::AbstractOperator*>>
        aops_gen)
    : id_map_(id_map)
    , state_mapper_(state_mapper)
    , values_(state_mapper->get_variables().size(), 0)
    , aops_gen_(aops_gen)
{
}

void
TransitionGenerator<const pdbs::AbstractOperator*>::operator()(
    const StateID& state,
    const pdbs::AbstractOperator* op,
    Distribution<StateID>& result)
{
    pdbs::AbstractState abstract_state = id_map_.get_state(state);
    for (auto it = op->outcomes.begin(); it != op->outcomes.end(); it++) {
        const pdbs::AbstractState succ = abstract_state + it->first;
        result.add(id_map_.get_state_id(succ), it->second);
    }
}

void
TransitionGenerator<const pdbs::AbstractOperator*>::operator()(
    const StateID& state,
    std::vector<const pdbs::AbstractOperator*>& aops,
    std::vector<Distribution<StateID>>& result)
{
    pdbs::AbstractState abstract_state = id_map_.get_state(state);
    state_mapper_->to_values(abstract_state, values_);
    aops_gen_->generate_applicable_ops(values_, aops);
    result.resize(aops.size());
    for (int i = aops.size() - 1; i >= 0; --i) {
        const pdbs::AbstractOperator* op = aops[i];
        for (auto it = op->outcomes.begin(); it != op->outcomes.end(); it++) {
            const pdbs::AbstractState succ = abstract_state + it->first;
            result[i].add(id_map_.get_state_id(succ), it->second);
        }
    }
}

namespace pdbs {

AbstractStateInStoreEvaluator::AbstractStateInStoreEvaluator(
    const QualitativeResultStore* states,
    value_type::value_t value_in,
    value_type::value_t value_not_in)
    : states_(states)
    , value_in_(value_in)
    , value_not_in_(value_not_in)
{
}

EvaluationResult
AbstractStateInStoreEvaluator::evaluate(const AbstractState& state)
{
    const bool is_contained = states_->get(state);
    return EvaluationResult(
        is_contained, is_contained ? value_in_ : value_not_in_);
}

value_type::value_t
ZeroCostActionEvaluator::evaluate(const AbstractOperator*)
{
    return 0;
}

value_type::value_t
UnitCostActionEvaluator::evaluate(const AbstractOperator*)
{
    return -1;
}

value_type::value_t
NormalCostActionEvaluator::evaluate(const AbstractOperator* op)
{
    return -op->cost;
}

} // namespace pdbs
} // namespace probabilistic
