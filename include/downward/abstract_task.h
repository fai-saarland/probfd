#ifndef ABSTRACT_TASK_H
#define ABSTRACT_TASK_H

#include "downward/axiom_space.h"
#include "downward/fact_pair.h"
#include "downward/goal_fact_list.h"
#include "downward/operator_cost_function.h"
#include "downward/variable_space.h"

#include "downward/algorithms/subscriber.h"

#include <string>
#include <vector>

namespace downward {

class PlanningTask
    : public subscriber::SubscriberService<PlanningTask>
    , public VariableSpace
    , public AxiomSpace
    , public GoalFactList {
public:
    virtual std::string get_operator_name(int index) const = 0;
    virtual int get_num_operators() const = 0;
    virtual int get_num_operator_preconditions(int index) const = 0;
    virtual FactPair
    get_operator_precondition(int op_index, int fact_index) const = 0;

    virtual std::vector<int> get_initial_state_values() const = 0;
};

class AbstractTask
    : public PlanningTask
    , public OperatorCostFunction<int> {
public:
    virtual int get_num_operator_effects(int op_index) const = 0;
    virtual int
    get_num_operator_effect_conditions(int op_index, int eff_index) const = 0;
    virtual FactPair
    get_operator_effect_condition(int op_index, int eff_index, int cond_index)
        const = 0;
    virtual FactPair get_operator_effect(int op_index, int eff_index) const = 0;
};

} // namespace downward

#endif
