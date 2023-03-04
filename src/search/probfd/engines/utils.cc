#include "probfd/engines/utils.h"

#include <cassert>

namespace probfd {
namespace engines {

value_t as_lower_bound(Interval interval)
{
    return interval.lower;
}

value_t as_lower_bound(value_t single)
{
    return single;
}

void set_min(Interval& lhs, Interval rhs)
{
    set_min(lhs.lower, rhs.lower);
    set_min(lhs.upper, rhs.upper);
}

void set_min(value_t& lhs, value_t rhs)
{
    lhs = std::min(lhs, rhs);
}

bool update(Interval& lhs, Interval rhs, bool check_upper)
{
    const bool result = is_approx_greater(rhs.lower, lhs.lower) ||
                        (check_upper && is_approx_less(rhs.upper, lhs.upper));
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

} // namespace engines
} // namespace probfd