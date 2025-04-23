#ifndef OPERATOR_COST_H
#define OPERATOR_COST_H

#include "downward/operator_cost_function.h"

namespace downward {
class AxiomOrOperatorProxy;
class OperatorProxy;

enum OperatorCost : unsigned short {
    NORMAL = 0,
    ONE = 1,
    PLUSONE = 2,
    MAX_OPERATOR_COST
};

int get_adjusted_action_cost(
    const AxiomOrOperatorProxy& op,
    const OperatorIntCostFunction& cost_function,
    OperatorCost cost_type,
    bool is_unit_cost);

int get_adjusted_action_cost(
    const OperatorProxy& op,
    const OperatorIntCostFunction& cost_function,
    OperatorCost cost_type,
    bool is_unit_cost);

}

#endif
