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

    std::common_reference_t<
        const std::iter_value_t<std::ranges::iterator_t<Range>>&&,
        std::iter_reference_t<std::ranges::iterator_t<Range>>>
    operator[](int index) const
    {
        return operator_costs[index];
    }

    auto begin() { return std::ranges::begin(operator_costs); }

    auto end() { return std::ranges::end(operator_costs); }

    auto begin() const { return std::ranges::begin(operator_costs); }

    auto end() const { return std::ranges::end(operator_costs); }
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
