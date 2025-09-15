#ifndef EXPLICIT_TERMINATION_COSTS_H
#define EXPLICIT_TERMINATION_COSTS_H

#include "probfd/aliases.h"
#include "probfd/termination_costs.h"

namespace probfd::tasks {

class ExplicitTermininationCosts : public TerminationCosts {
    value_t goal_termination_cost;
    value_t non_goal_termination_cost;

public:
    ExplicitTermininationCosts(
        value_t goal_termination_cost,
        value_t non_goal_termination_cost)
        : goal_termination_cost(goal_termination_cost)
        , non_goal_termination_cost(non_goal_termination_cost)
    {
    }

    value_t get_goal_termination_cost() const override
    {
        return goal_termination_cost;
    }

    value_t get_non_goal_termination_cost() const override
    {
        return non_goal_termination_cost;
    }
};

} // namespace probfd::tasks

#endif // EXPLICIT_TERMINATION_COSTS_H
