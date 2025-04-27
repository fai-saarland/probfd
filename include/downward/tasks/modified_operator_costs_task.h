#ifndef TASKS_MODIFIED_OPERATOR_COSTS_TASK_H
#define TASKS_MODIFIED_OPERATOR_COSTS_TASK_H

#include "downward/operator_cost_function.h"

#include <ranges>
#include <vector>

namespace downward::extra_tasks {

template <typename CostType>
class VectorOperatorCostFunction : public OperatorCostFunction<CostType> {
    std::vector<CostType> operator_costs;

public:
    explicit VectorOperatorCostFunction(std::vector<CostType> costs)
        : operator_costs(std::move(costs))
    {
    }

    CostType get_operator_cost(int index) const override
    {
        return operator_costs[index];
    }

    CostType& operator[](int index) { return operator_costs[index]; }

    const CostType& operator[](int index) const
    {
        return operator_costs[index];
    }

    void decrease_costs(std::span<const CostType> costs)
    {
        for (auto&& [cost, dec] : std::views::zip(operator_costs, costs)) {
            cost -= dec;
        }
    }
};

using VectorOperatorIntCostFunction = VectorOperatorCostFunction<int>;

} // namespace downward::extra_tasks

#endif
