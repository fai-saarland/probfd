#ifndef TASKS_EXPLICIT_INITIAL_STATE_VALUES_H
#define TASKS_EXPLICIT_INITIAL_STATE_VALUES_H

#include "downward/initial_state_values.h"

#include <vector>

namespace downward::extra_tasks {

class ExplicitInitialStateValues : public InitialStateValues {
    const std::vector<int> values;

public:
    explicit ExplicitInitialStateValues(std::vector<int> values)
        : values(std::move(values))
    {
    }

    std::vector<int> get_initial_state_values() const override
    {
        return values;
    }
};

} // namespace downward::extra_tasks

#endif
