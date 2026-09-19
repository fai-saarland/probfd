#include "probfd/value_type.h"

#include <cassert>
#include <cstdlib>

namespace probfd {

value_t fraction_to_value(int numerator, int denominator)
{
    return static_cast<value_t>(numerator) / static_cast<value_t>(denominator);
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

bool is_approx_equal(value_t v1, value_t v2, value_t epsilon)
{
    assert(epsilon >= 0.0_vt);
    return v1 == v2 || std::abs(v1 - v2) <= epsilon;
}

bool is_approx_less(value_t v1, value_t v2, value_t epsilon)
{
    assert(epsilon >= 0.0_vt);
    return v1 + epsilon < v2;
}

bool is_approx_greater(value_t v1, value_t v2, value_t epsilon)
{
    assert(epsilon >= 0.0_vt);
    return v1 - epsilon > v2;
}

bool is_approx_zero(value_t v, value_t epsilon)
{
    assert(epsilon >= 0.0_vt);
    return v == 0_vt || std::abs(v) <= epsilon;
}

} // namespace probfd
