#ifndef PROBFD_VALUE_TYPE_H
#define PROBFD_VALUE_TYPE_H

#include <string>
#include <limits>

/// The top-level namespace of probabilistic Fast Downward.
namespace probfd {

/// Typedef for the state value type
using value_t = double;

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
    return double_to_value(value);
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

/**
 * @brief Compares two state values approximately with repect to a given
 * tolerance.
 *
 * @param tolerance - A tolerance parameter \f$\epsilon\f$ for the approximate
 * comparison
 *
 * @returns
 * - \b 0 if \f$|v_1 - v_2| \leq \epsilon\f$
 * - \b 1 if \f$v_1 - v_2 > \epsilon\f$
 * - \b -1 if \f$v_2 - v_1 > \epsilon\f$
 */
int approx_compare(value_t v1, value_t v2, value_t tolerance = g_epsilon);

} // namespace probfd

#endif // __VALUE_TYPE_H__
