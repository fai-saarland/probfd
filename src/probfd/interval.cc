#include "probfd/interval.h"

#include <ostream>

using namespace downward;

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

Interval& Interval::operator*=(value_t scale_factor)
{
    lower *= scale_factor;
    upper *= scale_factor;
    return *this;
}

Interval& Interval::operator/=(value_t dividend)
{
    lower /= dividend;
    upper /= dividend;
    return *this;
}

double Interval::length() const
{
    // Handles infinities!
    if (upper == lower) {
        return 0.0;
    }
    return upper - lower;
}

bool Interval::bounds_approximately_equal(value_t tolerance) const
{
    return is_approx_equal(lower, upper, tolerance);
}

Interval operator+(Interval lhs, Interval rhs)
{
    return Interval(lhs.lower + rhs.lower, lhs.upper + rhs.upper);
}

Interval operator*(value_t lhs, Interval rhs)
{
    return Interval(lhs * rhs.lower, lhs * rhs.upper);
}

Interval operator*(Interval lhs, value_t rhs)
{
    return Interval(lhs.lower * rhs, lhs.upper * rhs);
}

Interval operator/(Interval lhs, value_t rhs)
{
    return Interval(lhs.lower / rhs, lhs.upper / rhs);
}

std::ostream& operator<<(std::ostream& os, Interval val)
{
    return os << "[" << val.lower << ", " << val.upper << "]";
}

} // namespace probfd