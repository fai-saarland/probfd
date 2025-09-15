#ifndef PROBFD_TERMINATION_COSTS
#define PROBFD_TERMINATION_COSTS

#include "probfd/aliases.h"

namespace probfd {

class TerminationCosts {
public:
    virtual ~TerminationCosts() = default;

    virtual value_t get_goal_termination_cost() const = 0;
    virtual value_t get_non_goal_termination_cost() const = 0;
};

} // namespace probfd

#endif
