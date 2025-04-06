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

Interval operator*(value_t val, Interval rhs)
{
    return Interval(val * rhs.lower, val * rhs.upper);
}

Interval operator*(Interval rhs, value_t val)
{
    return Interval(rhs.lower * val, rhs.upper * val);
}

Interval operator/(Interval rhs, value_t dividend)
{
    return Interval(rhs.lower / dividend, rhs.upper / dividend);
}

std::ostream& operator<<(std::ostream& out, Interval value)
{
    return out << "[" << value.lower << ", " << value.upper << "]";
}

} // namespace probfd