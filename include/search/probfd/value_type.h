#ifndef MDPS_VALUE_TYPE_H
#define MDPS_VALUE_TYPE_H

#include <string>

/// The main namespace of the probabilistic Fast Downward implementations.
namespace probfd {

/// Namespace dedicated to state value types.
namespace value_type {

/// Typedef for the state value type
using value_t = double;

extern const value_t one;  ///< A constant representing state value one.
extern const value_t zero; ///< A constant representing state value zero.
extern const value_t inf;  ///< A constant representing state value +infinity.

/// A constant representing the machine epsilon for the value type.
extern const value_t eps;

/// The global epsilon precision value.
extern value_t g_epsilon;

extern value_t from_double(double d);
extern value_t from_fraction(int nom, int denom);
extern value_t from_string(const std::string& str);
extern value_t abs(value_t val);

bool is_approx_equal(value_t v1, value_t v2, value_t tolerance = g_epsilon);
bool is_approx_less(value_t v1, value_t v2, value_t tolerance = g_epsilon);
bool is_approx_greater(value_t v1, value_t v2, value_t tolerance = g_epsilon);

} // namespace value_type
} // namespace probfd

#endif // __VALUE_TYPE_H__
