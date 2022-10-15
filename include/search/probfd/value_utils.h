#ifndef MDPS_VALUE_UTILS_H
#define MDPS_VALUE_UTILS_H

#include "value_type.h"

#include <ostream>
#include <type_traits>
#include <utility>

namespace probfd {
namespace value_utils {

/**
 * @brief Represents an interval of state values as a pair of lower and upper
 * bound.
 */
struct IntervalValue {
    value_type::value_t lower; ///< Lower bound of the interval
    value_type::value_t upper; ///< Upper bound of the interval

    /**
     * @brief Constructs a singleton interval with specified lower and upper
     * bound.
     *
     * @param val - The only element of the interval.
     */
    explicit IntervalValue(value_type::value_t val = value_type::zero);

    /**
     * @brief Constructs an interval from a lower and upper bound.
     *
     * @param lb - Lower bound of the interval.
     * @param ub - Upper bound of the interval.
     */
    explicit IntervalValue(value_type::value_t lb, value_type::value_t ub);

    /**
     * @brief Adds another interval by componentwise addition of its interval
     * bounds to this objects own interval bounds.
     */
    IntervalValue& operator+=(const IntervalValue& rhs);

    /**
     * @brief Substracts another interval by componentwise substraction of its
     * interval bounds from this objects own interval bounds.
     */
    IntervalValue& operator-=(const IntervalValue& rhs);

    /**
     * @brief Scales the interval bounds by muliplying with a factor.
     */
    IntervalValue& operator*=(value_type::value_t scale_factor);

    /**
     * @brief Scales down the interval bounds by dividing with a factor.
     */
    IntervalValue& operator/=(value_type::value_t inv_scale_factor);

    /**
     * @brief Computes the component-wise addition of two intervals.
     */
    friend IntervalValue
    operator+(const IntervalValue& lhs, const IntervalValue& rhs);

    /**
     * @brief Computes the component-wise substraction of two intervals.
     */
    friend IntervalValue
    operator-(const IntervalValue& lhs, const IntervalValue& rhs);

    /**
     * @brief Computes an interval with scaled bounds.
     */
    friend IntervalValue
    operator*(value_type::value_t scale_factor, const IntervalValue& rhs);

    /**
     * @copydoc operator*(value_type::value_t, const IntervalValue&)
     */
    friend IntervalValue
    operator*(const IntervalValue& lhs, value_type::value_t scale_factor);

    /**
     * @brief Computes an interval with bounds inversely scaled by some factor.
     */
    friend IntervalValue
    operator/(const IntervalValue& lhs, value_type::value_t inv_scale_factor);

    /**
     * @brief Compares the lower bounds of two intervals.
     *
     * @note This method does not use floating-point comparison, but
     * instead considers floating points which are value_type::g_epsilon
     * -close to each other as equal.
     *
     * @returns 1 - If the lower bound of the left interval is approximately
     * greater then the lower bound of the second interval
     * @returns -1 - If the lower bound of the left interval is approximately
     * smaller then the lower bound of the second interval
     * @returns 0 - Otherwise, i.e. the lower bounds of the left interval and
     * right interval are approximately equal
     *
     * @todo Rename to `approx_compare_lower_bounds`
     */
    friend int compare(const IntervalValue& lhs, const IntervalValue& rhs);

    /**
     * @brief Calls `IntervalValue::update(lhs, rhs, true)`.
     *
     * @see update(IntervalValue&, const IntervalValue&, bool)
     */
    friend bool update(IntervalValue& lhs, const IntervalValue& rhs);

    /**
     * @brief Intersects two intervals and assigns the result to the left
     * operand.
     *
     * @warning It is undefined behaviour to call this function with two
     * non-intersecting intervals.
     *
     * @returns \b true if the lower bound of the left operand changed by more
     * than value_type::g_epsilon or if check_upper is true and the greater
     * bound of the left operand changed by more than epsilon. \b false
     * otherwise.
     */
    friend bool
    update(IntervalValue& lhs, const IntervalValue& rhs, bool check_upper);

    /**
     * @brief Returns the distance between the upper and lower bound.
     *
     * @todo Rename to `gap`, `distance` etc.
     */
    double error_bound() const { return upper - lower; }

    /**
     * @brief Checks if the bounds of the interval are approximately equal.
     *
     * @note This method does not check for exact floating-point equality, but
     * instead checks whether the bounds are farther than value_type::g_epsilon
     * apart.
     *
     * @todo Rename this method to bounds_approx_equal and make the precision
     * a parameter, defaulting to value_type::g_epsilon.
     */
    bool bounds_equal() const
    {
        return value_type::approx_equal()(lower, upper);
    }

    /**
     * @brief Checks for floating-point equality of the respective bounds.
     */
    friend bool operator==(const IntervalValue& lhs, const IntervalValue& rhs);

    /**
     * @brief Stream output operator.
     */
    friend std::ostream& operator<<(std::ostream&, const IntervalValue&);
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
} // namespace probfd

#endif // __VALUE_UTILS_H__