#include "dead_end_listener.h"

namespace probabilistic {

DeadEndListener<GlobalState, const ProbabilisticOperator*>::DeadEndListener(
    std::shared_ptr<state_component::StateComponentListener> listener,
    StateIDMap<GlobalState>* state_id_map,
    TransitionGenerator<const ProbabilisticOperator*>* transition_gen)
    : listener_(listener)
    , state_id_map_(state_id_map)
    , transition_gen_(transition_gen)
{
}

bool DeadEndListener<GlobalState, const ProbabilisticOperator*>::
    requires_neighbors() const
{
    return listener_->requires_recognized_neighbors_explicitly();
}

bool DeadEndListener<GlobalState, const ProbabilisticOperator*>::operator()(
    const StateID& state)
{
    component_.insert(state_id_map_->get_state(state));
    const bool r = notify();
    component_.clear();
    successors_.clear();
    return r;
}

bool DeadEndListener<GlobalState, const ProbabilisticOperator*>::operator()(
    typename std::vector<StateID>::const_iterator begin,
    typename std::vector<StateID>::const_iterator end)
{
    for (auto it = begin; it != end; ++it) {
        component_.insert(state_id_map_->get_state(*it));
    }
    const bool r = notify();
    component_.clear();
    successors_.clear();
    return r;
}

bool DeadEndListener<GlobalState, const ProbabilisticOperator*>::operator()(
    typename std::vector<StateID>::const_iterator begin,
    typename std::vector<StateID>::const_iterator end,
    typename std::vector<StateID>::const_iterator rn_begin,
    typename std::vector<StateID>::const_iterator rn_end)
{
    for (auto it = begin; it != end; ++it) {
        component_.insert(state_id_map_->get_state(*it));
    }
    for (auto it = rn_begin; it != rn_end; ++it) {
        successors_.insert(state_id_map_->get_state(*it));
    }
    using iterator = std::unordered_set<GlobalState>::const_iterator;
    state_component::StateComponentIterator<iterator> component(
        component_.begin(),
        component_.end());
    state_component::StateComponentIterator<iterator> neighbors(
        successors_.begin(),
        successors_.end());
    const bool r =
        listener_->notify_new_component(component, neighbors) !=
        state_component::StateComponentListener::Status::StopListening;
    component_.clear();
    successors_.clear();
    return r;
}

bool DeadEndListener<GlobalState, const ProbabilisticOperator*>::notify()
{
    if (listener_->requires_recognized_neighbors_explicitly()) {
        feed_neighbors();
    }
    using iterator = std::unordered_set<GlobalState>::const_iterator;
    state_component::StateComponentIterator<iterator> component(
        component_.begin(),
        component_.end());
    state_component::StateComponentIterator<iterator> neighbors(
        successors_.begin(),
        successors_.end());
    return listener_->notify_new_component(component, neighbors) !=
           state_component::StateComponentListener::Status::StopListening;
}

void DeadEndListener<GlobalState, const ProbabilisticOperator*>::
    feed_neighbors()
{
    for (auto it = component_.begin(); it != component_.end(); ++it) {
        transition_gen_->operator()(
            state_id_map_->get_state_id(*it),
            aops_,
            transitions_);
        aops_.clear();
        for (int i = transitions_.size() - 1; i >= 0; --i) {
            const auto& t = transitions_[i];
            for (auto succ = t.begin(); succ != t.end(); ++succ) {
                GlobalState succ_state = state_id_map_->get_state(succ->first);
                if (!component_.count(succ_state)) {
                    successors_.insert(succ_state);
                }
            }
        }
        transitions_.clear();
    }
}

void DeadEndListener<GlobalState, const ProbabilisticOperator*>::
    print_statistics(std::ostream& out) const
{
    out << std::endl << "Dead End Learning Statistics:" << std::endl;
    listener_->print_statistics();
}

} // namespace probabilistic
