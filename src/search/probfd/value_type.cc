#include "probfd/value_type.h"

#include <cassert>
#include <cmath>

namespace probfd {

value_t g_epsilon = 5e-5;

value_t fraction_to_value(int nom, int denom)
{
    return static_cast<value_t>(nom) / static_cast<value_t>(denom);
}

value_t string_to_value(const std::string& str)
{
    for (unsigned i = 0; i < str.size(); ++i) {
        if (str[i] == '/') {
            return fraction_to_value(
                std::stoi(str.substr(0, i)),
                std::stoi(str.substr(i + 1)));
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
    assert(tolerance >= 0.0_vt);
    return v1 == v2 || std::abs(v1 - v2) <= tolerance;
}

bool is_approx_less(value_t v1, value_t v2, value_t tolerance)
{
    assert(tolerance >= 0.0_vt);
    return v1 + tolerance < v2;
}

bool is_approx_greater(value_t v1, value_t v2, value_t tolerance)
{
    assert(tolerance >= 0.0_vt);
    return v1 - tolerance > v2;
}

} // namespace probfd
