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

/// The default tolerance value for approximate comparisons.
extern value_t g_epsilon;

value_t fraction_to_value(int nom, int denom);
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
bool is_approx_equal(value_t v1, value_t v2, value_t epsilon = g_epsilon);

/// Equivalent to \f$v_2 - v_1 > \epsilon\f$
bool is_approx_less(value_t v1, value_t v2, value_t epsilon = g_epsilon);

/// Equivalent to \f$v_1 - v_2 > \epsilon\f$
bool is_approx_greater(value_t v1, value_t v2, value_t epsilon = g_epsilon);

/// Unary function object for approximate equality comparison.
struct approx_eq_to {
    const value_t value;
    const value_t epsilon;

    explicit approx_eq_to(value_t value, value_t epsilon = g_epsilon)
        : value(value)
        , epsilon(epsilon)
    {
    }

    bool operator()(value_t other) const
    {
        return is_approx_equal(value, other);
    }
};

/// Unary function object for approximate inequality comparison.
struct approx_neq_to {
    const value_t value;
    const value_t epsilon;

    explicit approx_neq_to(value_t value, value_t epsilon = g_epsilon)
        : value(value)
        , epsilon(epsilon)
    {
    }

    bool operator()(value_t other) const
    {
        return !is_approx_equal(value, other);
    }
};

/// Unary function object for approximate inequality comparison.
struct approx_less {
    const value_t epsilon;

    explicit approx_less(value_t epsilon = g_epsilon)
        : epsilon(epsilon)
    {
    }

    bool operator()(value_t left, value_t right) const
    {
        return is_approx_less(left, right);
    }
};

} // namespace probfd

#endif // PROBFD_VALUE_TYPE_H
