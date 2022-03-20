#ifndef MDPS_DEAD_END_LISTENER_H
#define MDPS_DEAD_END_LISTENER_H

#include "../global_state.h"
#include "../state_component.h"
#include "../state_component_listener.h"

#include "engine_interfaces/dead_end_listener.h"
#include "probabilistic_operator.h"
#include "state_id_map.h"
#include "transition_generator.h"

#include <memory>
#include <unordered_set>
#include <vector>

class Heuristic;

namespace probabilistic {

template <>
class DeadEndListener<GlobalState, const ProbabilisticOperator*> {
public:
    explicit DeadEndListener(
        std::shared_ptr<state_component::StateComponentListener> listener,
        StateIDMap<GlobalState>* state_id_map,
        TransitionGenerator<const ProbabilisticOperator*>* transition_gen);

    bool operator()(const StateID& state);

    bool operator()(
        std::vector<StateID>::const_iterator begin,
        std::vector<StateID>::const_iterator end,
        std::vector<StateID>::const_iterator rn_begin,
        std::vector<StateID>::const_iterator rn_end);

    bool operator()(
        std::vector<StateID>::const_iterator begin,
        std::vector<StateID>::const_iterator end);

    void print_statistics(std::ostream& out) const;

    bool requires_neighbors() const;

protected:
    bool notify();

private:
    void feed_neighbors();

    std::shared_ptr<state_component::StateComponentListener> listener_;
    StateIDMap<GlobalState>* state_id_map_;
    TransitionGenerator<const ProbabilisticOperator*>* transition_gen_;
    std::vector<const ProbabilisticOperator*> aops_;
    std::vector<Distribution<StateID>> transitions_;
    std::unordered_set<GlobalState> component_;
    std::unordered_set<GlobalState> successors_;
};

} // namespace probabilistic

#endif // __DEAD_END_LISTENER_H__