#include "probfd/value_type.h"

#include <cmath>
#include <limits>

namespace probfd {
namespace value_type {

value_t from_double(double d)
{
    return d;
}

value_t from_fraction(int nom, int denom)
{
    return static_cast<value_t>(nom) / static_cast<value_t>(denom);
}

value_t from_string(const std::string& str)
{
    for (unsigned i = 0; i < str.size(); ++i) {
        if (str[i] == '/') {
            return from_fraction(
                    std::stoi(str.substr(0, i)),
                    std::stoi(str.substr(i+1)));
        }
    }
    return std::stod(str);
}

value_t abs(value_t val)
{
    return std::abs(val);
}

bool is_approx_equal(value_t v1, value_t v2, value_t tolerance)
{
    return std::abs(v1 - v2) <= tolerance;
}

bool is_approx_less(value_t v1, value_t v2, value_t tolerance)
{
    return v1 + tolerance <= v2;
}

bool is_approx_greater(value_t v1, value_t v2, value_t tolerance)
{
    return v1 - tolerance >= v2;
}

const value_t one = 1;
const value_t zero = 0;
const value_t inf = std::numeric_limits<double>::infinity();
const value_t eps = std::numeric_limits<double>::epsilon();

value_t g_epsilon = 5e-5;

} // namespace value_type
} // namespace probfd
