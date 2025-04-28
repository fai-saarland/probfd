#ifndef INITIAL_STATE_VALUES_H
#define INITIAL_STATE_VALUES_H

#include "downward/algorithms/subscriber.h"

#include <vector>

namespace downward {

class State;

class InitialStateValues
    : public subscriber::SubscriberService<InitialStateValues> {
public:
    virtual std::vector<int> get_initial_state_values() const = 0;

    State get_initial_state() const;
};

} // namespace downward

#endif
