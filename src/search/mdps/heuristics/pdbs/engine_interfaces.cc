#include "engine_interfaces.h"

#include "../../../successor_generator.h"
#include "../../probabilistic_operator.h"
#include "qualitative_result_store.h"

#include "../../../pdbs/pattern_database.h"
#include "expcost_projection.h"
#include "maxprob_projection.h"

#include <limits>

namespace probabilistic {

unsigned StateIDMap<pdbs::AbstractState>::size() const
{
    return seen.size();
}

StateIDMap<pdbs::AbstractState>::visited_iterator
StateIDMap<pdbs::AbstractState>::visited_begin() const
{
    return seen.cbegin();
}

StateIDMap<pdbs::AbstractState>::visited_iterator
StateIDMap<pdbs::AbstractState>::visited_end() const
{
    return seen.cend();
}

StateIDMap<pdbs::AbstractState>::visited_range
StateIDMap<pdbs::AbstractState>::visited() const
{
    return visited_range(visited_begin(), visited_end());
}

StateID
StateIDMap<pdbs::AbstractState>::get_state_id(const pdbs::AbstractState& state)
{
    seen.insert(state.id);
    return StateID(state.id);
}

pdbs::AbstractState
StateIDMap<pdbs::AbstractState>::get_state(const StateID& id)
{
    return pdbs::AbstractState(id);
}

ActionIDMap<const pdbs::AbstractOperator*>::ActionIDMap(
    const std::vector<pdbs::AbstractOperator>& ops)
    : ops_(ops)
{
}

ActionID ActionIDMap<const pdbs::AbstractOperator*>::get_action_id(
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
    , values_(state_mapper->get_pattern().size(), 0)
    , aops_gen_(aops_gen)
{
}

void ApplicableActionsGenerator<const pdbs::AbstractOperator*>::operator()(
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
    , values_(state_mapper->get_pattern().size(), 0)
    , aops_gen_(aops_gen)
{
}

void TransitionGenerator<const pdbs::AbstractOperator*>::operator()(
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

void TransitionGenerator<const pdbs::AbstractOperator*>::operator()(
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

AbstractStateInSetRewardFunction::AbstractStateInSetRewardFunction(
    const std::unordered_set<AbstractState>* states_,
    value_type::value_t value_in,
    value_type::value_t value_not_in)
    : states_(states_)
    , value_in_(value_in)
    , value_not_in_(value_not_in)
{
}

EvaluationResult
AbstractStateInSetRewardFunction::evaluate(const AbstractState& state)
{
    const bool is_contained = states_->find(state) != states_->end();
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

    this->domain_size = mapper->get_domain_size_raw(idx);
    this->left_multiplier = mapper->get_multiplier_raw(idx);
    this->right_multiplier =
        (idx + 1) < mapper->num_vars() ?
        mapper->get_multiplier_raw(idx + 1) :
        mapper->num_states();
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

value_type::value_t
ZeroCostActionEvaluator::evaluate(StateID, const AbstractOperator*)
{
    return 0;
}

value_type::value_t
UnitCostActionEvaluator::evaluate(StateID, const AbstractOperator*)
{
    return -1;
}

value_type::value_t
NormalCostActionEvaluator::evaluate(StateID, const AbstractOperator* op)
{
    return -op->cost;
}

} // namespace pdbs
} // namespace probabilistic
