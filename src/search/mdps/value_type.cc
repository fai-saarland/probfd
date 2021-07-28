#include "value_type.h"

#include <cmath>
#include <limits>

namespace probabilistic {
namespace value_type {

value_t from_double(double d)
{
    return d;
}

value_t from_fraction(int nom, int denom)
{
    return (((value_t)nom) / (value_t)denom);
}

value_t cap(const value_t& val, const value_t& maxval)
{
    return val < maxval ? val : maxval;
}

value_t abs(const value_t& val)
{
    return std::abs(val);
}

approx_equal::approx_equal(const value_t& epsilon)
    : eps_(epsilon)
{
}

approx_less::approx_less(const value_t& epsilon)
    : eps_(epsilon)
{
}

approx_greater::approx_greater(const value_t& epsilon)
    : eps_(epsilon)
{
}

bool approx_equal::operator()(const value_t& v1, const value_t& v2) const
{
    return std::abs(v1 - v2) <= eps_;
}

bool approx_less::operator()(const value_t& v1, const value_t& v2) const
{
    return v1 + eps_ <= v2;
}

bool approx_greater::operator()(const value_t& v1, const value_t& v2) const
{
    return v1 - eps_ >= v2;
}

const value_t one = 1;
const value_t zero = 0;
const value_t inf = std::numeric_limits<double>::infinity();
const value_t eps = std::numeric_limits<double>::epsilon();

value_t g_epsilon = 5e-5;

} // namespace value_type
} // namespace probabilistic
