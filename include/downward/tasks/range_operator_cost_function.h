#ifndef TASKS_MODIFIED_OPERATOR_COSTS_TASK_H
#define TASKS_MODIFIED_OPERATOR_COSTS_TASK_H

#include "downward/operator_cost_function.h"

#include <ranges>
#include <vector>

namespace downward::extra_tasks {

template <std::ranges::random_access_range Range>
    requires std::is_arithmetic_v<std::ranges::range_value_t<Range>>
class RangeOperatorCostFunction
    : public OperatorCostFunction<std::ranges::range_value_t<Range>> {
    Range operator_costs;

public:
    explicit RangeOperatorCostFunction()
        requires std::is_default_constructible_v<Range>
    = default;

    explicit RangeOperatorCostFunction(const Range& costs)
        requires std::copy_constructible<Range>
        : operator_costs(costs)
    {
    }

    explicit RangeOperatorCostFunction(Range&& costs)
        requires std::move_constructible<Range>
        : operator_costs(std::move(costs))
    {
    }

    std::ranges::range_value_t<Range>
    get_operator_cost(int index) const override
    {
        return operator_costs[index];
    }

    std::ranges::range_reference_t<Range> operator[](int index)
    {
        return operator_costs[index];
    }

    std::ranges::range_const_reference_t<Range> operator[](int index) const
    {
        return operator_costs[index];
    }

    template <std::ranges::random_access_range Range2>
        requires std::same_as<
            std::ranges::range_value_t<Range>,
            std::ranges::range_value_t<Range2>>
    void decrease_costs(Range2&& costs)
    {
        for (auto&& [cost, dec] : std::views::zip(operator_costs, costs)) {
            cost -= dec;
        }
    }
};

template <typename Range>
RangeOperatorCostFunction(Range&&...)
    -> RangeOperatorCostFunction<views::all_t<Range>>;

template <typename Range>
auto make_shared_range_cf(Range&& r)
{
    return std::make_shared<RangeOperatorCostFunction<views::all_t<Range>>>(
        std::forward<Range>(r));
}

template <typename T>
using VectorOperatorCostFunction = RangeOperatorCostFunction<std::vector<T>>;

using VectorOperatorIntCostFunction = VectorOperatorCostFunction<int>;

} // namespace downward::extra_tasks

#endif
