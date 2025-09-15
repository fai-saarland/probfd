#ifndef OPERATOR_COST_FUNCTION_H
#define OPERATOR_COST_FUNCTION_H

#include "downward/operator_cost_function_fwd.h"
#include "downward/operator_id.h"

namespace downward {

template <typename CostType>
class OperatorCostFunction {
public:
    virtual ~OperatorCostFunction() = default;

    virtual CostType get_operator_cost(int index) const = 0;

    CostType get_operator_cost(OperatorID index) const
    {
        return get_operator_cost(index.get_index());
    }
};

} // namespace downward

#endif
