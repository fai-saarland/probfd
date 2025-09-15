#ifndef OPERATOR_COST_FUNCTION_FWD_H
#define OPERATOR_COST_FUNCTION_FWD_H

namespace downward {

template <typename CostType>
class OperatorCostFunction;

using OperatorIntCostFunction = OperatorCostFunction<int>;

} // namespace downward

#endif
