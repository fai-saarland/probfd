#ifndef PROBFD_VALUE_TYPE_H
#define PROBFD_VALUE_TYPE_H

#include "probfd/aliases.h"

#include <limits>
#include <string>

/// The top-level namespace of probabilistic Fast Downward.
namespace probfd {

/// A constant representing state value \f$+\infty\f$.
static constexpr value_t INFINITE_VALUE =
    std::numeric_limits<double>::infinity();

value_t fraction_to_value(int numerator, int denominator);
value_t string_to_value(const std::string& str);
value_t abs(value_t val);

constexpr value_t double_to_value(double d)
{
    return d;
}

/// User-defined floating-point literals for state values.
constexpr value_t operator"" _vt(long double value)
{
    return double_to_value(static_cast<double>(value));
}

/// User-defined integer literals for state values.
constexpr value_t operator"" _vt(unsigned long long value)
{
    return double_to_value(static_cast<double>(value));
}

/// Equivalent to \f$|v_1 - v_2| \leq \epsilon\f$
bool is_approx_equal(value_t v1, value_t v2, value_t epsilon);

/// Equivalent to \f$v_2 - v_1 > \epsilon\f$
bool is_approx_less(value_t v1, value_t v2, value_t epsilon);

/// Equivalent to \f$v_1 - v_2 > \epsilon\f$
bool is_approx_greater(value_t v1, value_t v2, value_t epsilon);

} // namespace probfd

#endif // PROBFD_VALUE_TYPE_H
