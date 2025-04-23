#ifndef OPERATOR_COST_FUNCTION_H
#define OPERATOR_COST_FUNCTION_H

namespace downward {

template <typename CostType>
class OperatorCostFunction {
public:
    virtual ~OperatorCostFunction() = default;

    virtual CostType get_operator_cost(int index) const = 0;
};

using OperatorIntCostFunction = OperatorCostFunction<int>;

} // namespace downward

#endif
