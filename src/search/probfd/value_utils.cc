#include "probfd/value_utils.h"

#include <algorithm>
#include <cassert>

namespace probfd {

Interval::Interval(value_t val)
    : Interval(val, val)
{
}

Interval::Interval(value_t lb, value_t ub)
    : lower(lb)
    , upper(ub)
{
}

Interval& Interval::operator+=(Interval rhs)
{
    lower += rhs.lower;
    upper += rhs.upper;
    return *this;
}

Interval& Interval::operator-=(Interval rhs)
{
    lower -= rhs.lower;
    upper -= rhs.upper;
    return *this;
}

Interval& Interval::operator*=(value_t prob)
{
    lower *= prob;
    upper *= prob;
    return *this;
}

Interval& Interval::operator/=(value_t prob)
{
    lower /= prob;
    upper /= prob;
    return *this;
}

Interval operator+(Interval lhs, Interval rhs)
{
    return Interval(lhs.lower + rhs.lower, lhs.upper + rhs.upper);
}

Interval operator-(Interval lhs, Interval rhs)
{
    return Interval(lhs.lower - rhs.lower, lhs.upper - rhs.upper);
}

Interval operator*(value_t val, Interval rhs)
{
    return Interval(val * rhs.lower, val * rhs.upper);
}

Interval operator*(Interval rhs, value_t val)
{
    return Interval(rhs.lower * val, rhs.upper * val);
}

Interval operator/(Interval rhs, value_t val)
{
    assert(val != 0.0);
    return Interval(rhs.lower / val, rhs.upper / val);
}

int approx_compare(Interval lhs, Interval rhs, value_t epsilon)
{
    return approx_compare(lhs.upper, rhs.upper, epsilon);
}

int approx_compare(value_t v1, value_t v2, value_t epsilon)
{
    return is_approx_equal(v1, v2, epsilon) ? 0 : v1 > v2 ? 1 : -1;
}

bool update(Interval& lhs, Interval rhs)
{
    return update(lhs, rhs, true);
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

void set_min(Interval& new_value, Interval tval)
{
    new_value.lower = std::min(tval.lower, new_value.lower);
    new_value.upper = std::min(tval.upper, new_value.upper);
}

bool operator==(Interval lhs, Interval rhs)
{
    return lhs.lower == rhs.lower && lhs.upper == rhs.upper;
}

std::ostream& operator<<(std::ostream& out, Interval value)
{
    return out << "[" << value.lower << ", " << value.upper << "]";
}

value_t as_lower_bound(Interval interval)
{
    return interval.lower;
}

value_t as_upper_bound(Interval interval)
{
    return interval.upper;
}

bool update(value_t& lhs, value_t rhs)
{
    const bool result = !is_approx_equal(lhs, rhs);
    lhs = rhs;
    return result;
}

void set_min(value_t& new_value, value_t tval)
{
    new_value = std::min(tval, new_value);
}

value_t as_lower_bound(value_t single)
{
    return single;
}

value_t as_upper_bound(value_t single)
{
    return single;
}

} // namespace probfd