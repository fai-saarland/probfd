#ifndef INITIAL_STATE_VALUES_H
#define INITIAL_STATE_VALUES_H

#include <vector>

namespace downward {

class InitialStateValues {
public:
    virtual ~InitialStateValues() = default;

    virtual std::vector<int> get_initial_state_values() const = 0;
};

} // namespace downward

#endif
