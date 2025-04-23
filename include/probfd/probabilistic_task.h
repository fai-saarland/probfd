#ifndef PROBFD_PROBABILISTIC_TASK_H
#define PROBFD_PROBABILISTIC_TASK_H

#include "downward/abstract_task.h"
#include "downward/operator_cost_function.h"

#include "probfd/probabilistic_operator_space.h"
#include "probfd/termination_costs.h"


namespace probfd::causal_graph {
class ProbabilisticCausalGraph;
}

namespace probfd {

/**
 * @brief Represents a probabilistic planning task with axioms and conditional
 * effects.
 *
 * This class should not be used directly to access the information of a
 * probabilistic planning task. To this end, use ProbabilisticTaskProxy.
 *
 * @see ProbabilisticTaskProxy
 */
class ProbabilisticTask
    : public downward::PlanningTask
    , public ProbabilisticOperatorSpace
    , public downward::OperatorCostFunction<value_t>
    , public TerminationCosts {
public:
    const causal_graph::ProbabilisticCausalGraph& get_causal_graph() const;
};

} // namespace probfd

#endif
