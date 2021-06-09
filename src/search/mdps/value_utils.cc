#include "value_utils.h"

#include <cassert>
#include <algorithm>

namespace probabilistic {
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

IntervalValue&
IntervalValue::operator+=(const IntervalValue& rhs)
{
    lower += rhs.lower;
    upper += rhs.upper;
    return *this;
}

IntervalValue&
IntervalValue::operator-=(const IntervalValue& rhs)
{
    lower -= rhs.lower;
    upper -= rhs.upper;
    return *this;
}

IntervalValue&
IntervalValue::operator*=(value_t prob)
{
    lower *= prob;
    upper *= prob;
    return *this;
}

IntervalValue&
IntervalValue::operator/=(value_t prob)
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
    if (approx_greater(eps)(lhs.lower, rhs.lower))
        return 1;

    if (approx_equal(eps)(lhs.lower, rhs.lower))
        return 0;
    
    return -1;
}

bool update_check(IntervalValue& lhs, const IntervalValue& rhs)
{
    return update_check(lhs, rhs, true);
}

bool update_check(IntervalValue& lhs, const IntervalValue& rhs, bool check_upper)
{
    const bool result = !value_type::approx_equal()(lhs.lower, rhs.lower)
        || (check_upper && !value_type::approx_equal()(lhs.upper, rhs.upper));
    lhs.lower = rhs.lower;
    lhs.upper = rhs.upper;
    assert(!value_type::approx_less()(lhs.upper, lhs.lower));
    return result;
}

void update(IntervalValue& new_value, const IntervalValue& tval)
{
    new_value.lower = std::max(tval.lower, new_value.lower);
    new_value.upper = std::max(tval.upper, new_value.upper);
}

bool operator==(const IntervalValue& lhs, const IntervalValue& rhs)
{
    return lhs.lower == rhs.lower && lhs.upper == rhs.upper;
}

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

int compare(const SingleValue& lhs, const SingleValue& rhs)
{
    if (approx_greater(eps)(lhs.value, rhs.value))
        return 1;

    if (approx_equal(eps)(lhs.value, rhs.value))
        return 0;
    
    return -1;
}

bool update_check(SingleValue& lhs, const SingleValue& rhs)
{
    const bool result = !value_type::approx_equal()(lhs.value, rhs.value);
    lhs.value = rhs.value;
    return result;
}

SingleValue::operator value_type::value_t() const {
    return value;
}

void update(SingleValue& new_value, const SingleValue& tval)
{
    new_value.value = std::max(tval.value, new_value.value);
}

bool operator==(const SingleValue& lhs, const SingleValue& rhs)
{
    return lhs.value == rhs.value;
}

value_type::value_t
as_lower_bound(const value_utils::SingleValue& single) {
    return single.value;
}

value_type::value_t
as_lower_bound(const value_utils::IntervalValue& interval) {
    return interval.lower;
}

value_type::value_t
as_upper_bound(const value_utils::SingleValue& single) {
    return single.value;
}

value_type::value_t
as_upper_bound(const value_utils::IntervalValue& interval) {
    return interval.upper;
}

} // namespace value_utils
} // namespace probabilistic