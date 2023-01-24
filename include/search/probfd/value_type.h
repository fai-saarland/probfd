#ifndef PROBFD_VALUE_TYPE_H
#define PROBFD_VALUE_TYPE_H

#include <string>

/// The top-level namespace of probabilistic Fast Downward.
namespace probfd {

/// Typedef for the state value type
using value_t = double;

/// A constant representing state value plus infinity
static constexpr value_t INFINITE_VALUE =
    std::numeric_limits<double>::infinity();

/// The default tolerance value for approximate comparisons.
extern value_t g_epsilon;

extern value_t fraction_to_value(int nom, int denom);
extern value_t string_to_value(const std::string& str);
extern value_t abs(value_t val);

constexpr value_t double_to_value(double d)
{
    return d;
}

// User-defined floating-point literals for state values.
constexpr value_t operator"" _vt(long double value)
{
    return double_to_value(value);
}

// User-defined integer literals for state values.
constexpr value_t operator"" _vt(unsigned long long value)
{
    return double_to_value(static_cast<double>(value));
}

} // namespace probfd

#endif // __VALUE_TYPE_H__
