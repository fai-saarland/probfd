#include "probfd/algorithms/utils.h"

#include "probfd/interval.h"

#include <cassert>

namespace probfd::algorithms {

value_t as_lower_bound(Interval interval)
{
    return interval.lower;
}

value_t as_lower_bound(value_t single)
{
    return single;
}

Interval as_interval(value_t lower_bound)
{
    return Interval(lower_bound, INFINITE_VALUE);
}

Interval as_interval(Interval value)
{
    return value;
}

bool set_min(Interval& lhs, Interval rhs)
{
    set_min(lhs.upper, rhs.upper);
    return set_min(lhs.lower, rhs.lower);
}

bool set_min(value_t& lhs, value_t rhs)
{
    if (rhs < lhs) {
        lhs = rhs;
        return true;
    }

    return false;
}

bool set_max(Interval& lhs, Interval rhs)
{
    set_max(lhs.upper, rhs.upper);
    return set_max(lhs.lower, rhs.lower);
}

bool set_max(value_t& lhs, value_t rhs)
{
    if (rhs > lhs) {
        lhs = rhs;
        return true;
    }

    return false;
}

ValueUpdateResult update(Interval& lhs, Interval rhs, value_t epsilon)
{
    const bool result = !is_approx_equal(rhs.lower, lhs.lower, epsilon) ||
                        !is_approx_equal(rhs.upper, lhs.upper, epsilon);
    lhs = rhs;
    return {result, lhs.bounds_approximately_equal(epsilon)};
}

ValueUpdateResult update(value_t& lhs, value_t rhs, value_t epsilon)
{
    const bool result = !is_approx_equal(lhs, rhs, epsilon);
    lhs = rhs;
    return {result, !result};
}

} // namespace probfd::algorithms
