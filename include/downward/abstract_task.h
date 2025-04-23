#ifndef ABSTRACT_TASK_H
#define ABSTRACT_TASK_H

#include "downward/axiom_space.h"
#include "downward/goal_fact_list.h"
#include "downward/initial_state_values.h"
#include "downward/operator_cost_function.h"
#include "downward/classical_operator_space.h"
#include "downward/task_id.h"
#include "downward/variable_space.h"

#include "downward/algorithms/subscriber.h"

namespace downward::causal_graph {
class CausalGraph;
}

namespace downward {

class PlanningTask
    : public subscriber::SubscriberService<PlanningTask>
    , public VariableSpace
    , public AxiomSpace
    , public virtual OperatorSpace
    , public GoalFactList
    , public InitialStateValues {
public:
    TaskID get_id() const { return TaskID(this); }
};

class AbstractTask
    : public PlanningTask
    , public ClassicalOperatorSpace
    , public OperatorCostFunction<int> {
public:
    const causal_graph::CausalGraph& get_causal_graph() const;
};

} // namespace downward

#endif
