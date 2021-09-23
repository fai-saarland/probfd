#ifndef MDPS_VALUE_UTILS_H
#define MDPS_VALUE_UTILS_H

#include "value_type.h"

#include <type_traits>
#include <utility>

namespace probabilistic {
namespace value_utils {

struct IntervalValue {
    value_type::value_t lower;
    value_type::value_t upper;

    explicit IntervalValue(value_type::value_t val = value_type::zero);

    explicit IntervalValue(value_type::value_t lb, value_type::value_t ub);

    IntervalValue& operator+=(const IntervalValue& rhs);
    IntervalValue& operator-=(const IntervalValue& rhs);

    IntervalValue& operator*=(value_type::value_t val);
    IntervalValue& operator/=(value_type::value_t val);

    friend IntervalValue
    operator+(const IntervalValue& lhs, const IntervalValue& rhs);

    friend IntervalValue
    operator-(const IntervalValue& lhs, const IntervalValue& rhs);

    friend IntervalValue
    operator*(value_type::value_t val, const IntervalValue& rhs);

    friend IntervalValue
    operator*(const IntervalValue& rhs, value_type::value_t val);

    friend IntervalValue
    operator/(const IntervalValue& rhs, value_type::value_t val);

    friend int compare(const IntervalValue& lhs, const IntervalValue& rhs);

    friend bool update(IntervalValue& lhs, const IntervalValue& rhs);

    friend bool
    update(IntervalValue& lhs, const IntervalValue& rhs, bool ign_upper);

    double error_bound() const { return upper - lower; }

    bool bounds_equal() const
    {
        return value_type::approx_equal()(lower, upper);
    }

    friend bool operator==(const IntervalValue& lhs, const IntervalValue& rhs);
};

/*
struct SingleValue {
    using DualBounds = std::false_type;

    value_type::value_t value;

    explicit SingleValue(value_type::value_t val = value_type::zero);

    SingleValue& operator+=(const SingleValue& rhs);
    SingleValue& operator-=(const SingleValue& rhs);

    SingleValue& operator*=(value_type::value_t val);
    SingleValue& operator/=(value_type::value_t val);

    friend SingleValue
    operator+(const SingleValue& lhs, const SingleValue& rhs);

    friend SingleValue
    operator-(const SingleValue& lhs, const SingleValue& rhs);

    friend SingleValue
    operator*(value_type::value_t val, const SingleValue& rhs);

    friend SingleValue
    operator*(const SingleValue& rhs, value_type::value_t val);

    friend SingleValue
    operator/(const SingleValue& rhs, value_type::value_t val);

    friend int
    compare(const SingleValue& lhs, const SingleValue& rhs);

    friend bool
    update_check(SingleValue& lhs, const SingleValue& rhs);

    operator value_type::value_t() const;

    friend bool operator==(const SingleValue& lhs, const SingleValue& rhs);
};
*/

template <typename Dual>
using IncumbentSolution =
    std::conditional_t<Dual::value, IntervalValue, value_type::value_t>;

// Comparisons

int compare(const IntervalValue& value0, const IntervalValue& value1);
int compare(
    const value_type::value_t& value0,
    const value_type::value_t& value1);

// Value update (old)

void set_max(IntervalValue& new_value, const IntervalValue& tval);
void set_max(value_type::value_t& new_value, const value_type::value_t& tval);

// Value update

bool update(IntervalValue& lhs, const IntervalValue& rhs);
bool update(IntervalValue& lhs, const IntervalValue& rhs, bool check_upper);
bool update(value_type::value_t& lhs, const value_type::value_t& rhs);

value_type::value_t as_lower_bound(const value_type::value_t& single);
value_type::value_t as_lower_bound(const IntervalValue& interval);

value_type::value_t as_upper_bound(const value_type::value_t& single);
value_type::value_t as_upper_bound(const IntervalValue& interval);

} // namespace value_utils
} // namespace probabilistic

#endif // __VALUE_UTILS_H__