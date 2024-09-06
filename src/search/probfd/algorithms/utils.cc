#include "probfd/algorithms/utils.h"

#include "probfd/interval.h"

#include <algorithm>
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

bool update(Interval& lhs, Interval rhs)
{
    const bool result = is_approx_greater(rhs.lower, lhs.lower) ||
                        is_approx_less(rhs.upper, lhs.upper);
    lhs.lower = std::max(lhs.lower, rhs.lower);
    lhs.upper = std::min(lhs.upper, rhs.upper);
    assert(!is_approx_less(lhs.upper, lhs.lower));
    return result;
}

bool update(value_t& lhs, value_t rhs)
{
    const bool result = !is_approx_equal(lhs, rhs);
    lhs = rhs;
    return result;
}

} // namespace probfd::algorithms
