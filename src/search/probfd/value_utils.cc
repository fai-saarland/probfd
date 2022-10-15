#include "probfd/value_utils.h"

#include <algorithm>
#include <cassert>

namespace probfd {
namespace value_utils {

using namespace value_type;

IntervalValue::IntervalValue(value_type::value_t val)
    : IntervalValue(val, val)
{
}

IntervalValue::IntervalValue(value_type::value_t lb, value_type::value_t ub)
    : lower(lb)
    , upper(ub)
{
}

IntervalValue& IntervalValue::operator+=(const IntervalValue& rhs)
{
    lower += rhs.lower;
    upper += rhs.upper;
    return *this;
}

IntervalValue& IntervalValue::operator-=(const IntervalValue& rhs)
{
    lower -= rhs.lower;
    upper -= rhs.upper;
    return *this;
}

IntervalValue& IntervalValue::operator*=(value_t prob)
{
    lower *= prob;
    upper *= prob;
    return *this;
}

IntervalValue& IntervalValue::operator/=(value_t prob)
{
    lower /= prob;
    upper /= prob;
    return *this;
}

IntervalValue operator+(const IntervalValue& lhs, const IntervalValue& rhs)
{
    return IntervalValue(lhs.lower + rhs.lower, lhs.upper + rhs.upper);
}

IntervalValue operator-(const IntervalValue& lhs, const IntervalValue& rhs)
{
    return IntervalValue(lhs.lower - rhs.lower, lhs.upper - rhs.upper);
}

IntervalValue operator*(value_t val, const IntervalValue& rhs)
{
    return IntervalValue(val * rhs.lower, val * rhs.upper);
}

IntervalValue operator*(const IntervalValue& rhs, value_t val)
{
    return IntervalValue(rhs.lower * val, rhs.upper * val);
}

IntervalValue operator/(const IntervalValue& rhs, value_t val)
{
    assert(val != 0.0);
    return IntervalValue(rhs.lower / val, rhs.upper / val);
}

int compare(const IntervalValue& lhs, const IntervalValue& rhs)
{
    if (approx_greater(eps)(lhs.lower, rhs.lower)) return 1;

    if (approx_equal(eps)(lhs.lower, rhs.lower)) return 0;

    return -1;
}

bool update(IntervalValue& lhs, const IntervalValue& rhs)
{
    return update(lhs, rhs, true);
}

bool update(IntervalValue& lhs, const IntervalValue& rhs, bool check_upper)
{
    const bool result =
        value_type::approx_greater()(rhs.lower, lhs.lower) ||
        (check_upper && value_type::approx_less()(rhs.upper, lhs.upper));
    lhs.lower = std::max(lhs.lower, rhs.lower);
    lhs.upper = std::min(lhs.upper, rhs.upper);
    assert(!value_type::approx_less()(lhs.upper, lhs.lower));
    return result;
}

void set_max(IntervalValue& new_value, const IntervalValue& tval)
{
    new_value.lower = std::max(tval.lower, new_value.lower);
    new_value.upper = std::max(tval.upper, new_value.upper);
}

bool operator==(const IntervalValue& lhs, const IntervalValue& rhs)
{
    return lhs.lower == rhs.lower && lhs.upper == rhs.upper;
}

std::ostream& operator<<(std::ostream& out, const IntervalValue& value)
{
    return out << "[" << value.lower << ", " << value.upper << "]";
}

value_type::value_t as_lower_bound(const value_utils::IntervalValue& interval)
{
    return interval.lower;
}

value_type::value_t as_upper_bound(const value_utils::IntervalValue& interval)
{
    return interval.upper;
}

/*
SingleValue::SingleValue(value_type::value_t val)
    : value(val)
{
}

SingleValue&
SingleValue::operator+=(const SingleValue& rhs)
{
    value += rhs.value;
    return *this;
}

SingleValue&
SingleValue::operator-=(const SingleValue& rhs)
{
    value -= rhs.value;
    return *this;
}

SingleValue&
SingleValue::operator*=(value_t prob)
{
    value *= prob;
    return *this;
}

SingleValue&
SingleValue::operator/=(value_t prob)
{
    value /= prob;
    return *this;
}

SingleValue operator+(const SingleValue& lhs, const SingleValue& rhs)
{
    return SingleValue(lhs.value + rhs.value);
}

SingleValue operator-(const SingleValue& lhs, const SingleValue& rhs)
{
    return SingleValue(lhs.value - rhs.value);
}

SingleValue operator*(value_t val, const SingleValue& rhs)
{
    return SingleValue(val * rhs.value);
}

SingleValue operator*(const SingleValue& rhs, value_t val)
{
    return SingleValue(rhs.value * val);
}

SingleValue operator/(const SingleValue& rhs, value_t val)
{
    return SingleValue(rhs.value / val);
}

*/

int compare(const value_type::value_t& lhs, const value_type::value_t& rhs)
{
    if (approx_greater(eps)(lhs, rhs)) return 1;

    if (approx_equal(eps)(lhs, rhs)) return 0;

    return -1;
}

bool update(value_type::value_t& lhs, const value_type::value_t& rhs)
{
    const bool result = !value_type::approx_equal()(lhs, rhs);
    lhs = rhs;
    return result;
}

/*

SingleValue::operator value_type::value_t() const {
    return value;
}

*/

void set_max(value_type::value_t& new_value, const value_type::value_t& tval)
{
    new_value = std::max(tval, new_value);
}

/*

bool operator==(const SingleValue& lhs, const SingleValue& rhs)
{
    return lhs.value == rhs.value;
}

*/

value_type::value_t as_lower_bound(const value_type::value_t& single)
{
    return single;
}

value_type::value_t as_upper_bound(const value_type::value_t& single)
{
    return single;
}

} // namespace value_utils
} // namespace probfd