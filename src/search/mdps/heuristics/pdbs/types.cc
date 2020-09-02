#include "types.h"

#include "../../../successor_generator.h"
#include "../../probabilistic_operator.h"
#include "qualitative_result_store.h"

namespace probabilistic {
namespace pdbs {

AbstractOperator::AbstractOperator(unsigned id)
    : original_operator_id(id)
    , pre(0)
{
}

AbstractOperatorToString::AbstractOperatorToString(
    const std::vector<ProbabilisticOperator>* ops)
    : ops_(ops)
{
}

std::string
AbstractOperatorToString::operator()(const AbstractOperator* op)
{
    return ops_->operator[](op->original_operator_id).get_name();
}

} // namespace pdbs

namespace algorithms {

ApplicableActionsGenerator<pdbs::AbstractState, const pdbs::AbstractOperator*>::
    ApplicableActionsGenerator(
        std::shared_ptr<pdbs::AbstractStateMapper> state_mapper,
        std::shared_ptr<successor_generator::SuccessorGenerator<
            const pdbs::AbstractOperator*>> aops_gen)
    : state_mapper_(state_mapper)
    , values_(state_mapper->get_variables().size(), 0)
    , aops_gen_(aops_gen)
{
}

void
ApplicableActionsGenerator<pdbs::AbstractState, const pdbs::AbstractOperator*>::
operator()(
    const pdbs::AbstractState& abstract_state,
    std::vector<const pdbs::AbstractOperator*>& aops)
{
    state_mapper_->to_values(abstract_state, values_);
    aops_gen_->generate_applicable_ops(values_, aops);
}

Distribution<pdbs::AbstractState>
TransitionGenerator<pdbs::AbstractState, const pdbs::AbstractOperator*>::
operator()(const pdbs::AbstractState& state, const pdbs::AbstractOperator* op)
    const
{
    Distribution<pdbs::AbstractState> result;
    for (auto it = op->outcomes.begin(); it != op->outcomes.end(); it++) {
        result.add(state + it->first, it->second);
    }
    return result;
}

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
StateIDMap<pdbs::AbstractState>::operator[](const pdbs::AbstractState& state)
{
    auto it = id_map_.emplace(state.id, next_id_);
    if (it.second) {
        ++next_id_;
        inverse_map_.emplace_back(state.id);
    }
    return StateID(it.first->second);
}

pdbs::AbstractState
StateIDMap<pdbs::AbstractState>::operator[](const StateID& id)
{
    assert(id.hash() < inverse_map_.size());
    return pdbs::AbstractState(inverse_map_[id.hash()]);
}

} // namespace algorithms

namespace pdbs {

AbstractStateInStore::AbstractStateInStore(
    QualitativeResultStore* states,
    value_type::value_t value_in,
    value_type::value_t value_not_in)
    : states_(states)
    , value_in_(value_in)
    , value_not_in_(value_not_in)
{
}

EvaluationResult
AbstractStateInStore::evaluate(const AbstractState& state)
{
    const bool is_contained = states_->get(state);
    return EvaluationResult(
        is_contained, is_contained ? value_in_ : value_not_in_);
}

value_type::value_t
AbstractTransitionNoReward::evaluate(
    const AbstractState&,
    const AbstractOperator*)
{
    return value_type::zero;
}

} // namespace pdbs
} // namespace probabilistic
