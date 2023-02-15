#ifndef PROBFD_VALUE_UTILS_H
#define PROBFD_VALUE_UTILS_H

#include "value_type.h"

#include <ostream>
#include <type_traits>
#include <utility>

namespace probfd {

/// Equivalent to \f$|v_1 - v_2| \leq \epsilon\f$
bool is_approx_equal(value_t v1, value_t v2, value_t epsilon = g_epsilon);

/// Equivalent to \f$v_2 - v_1 > \epsilon\f$
bool is_approx_less(value_t v1, value_t v2, value_t epsilon = g_epsilon);

/// Equivalent to \f$v_1 - v_2 > \epsilon\f$
bool is_approx_greater(value_t v1, value_t v2, value_t epsilon = g_epsilon);

/// Represents a closed interval over the extended reals.
struct Interval {
    value_t lower; ///< The Lower bound of the interval
    value_t upper; ///< The upper bound of the interval

    /// Constructs an interval consisting of a single point p.
    explicit Interval(value_t val = 0_vt);

    /// Constructs an interval from a specified lower and upper bound.
    explicit Interval(value_t lb, value_t ub);

    /// Equivalent to *this = *this + rhs
    Interval& operator+=(Interval rhs);

    /// Equivalent to *this = *this - rhs
    Interval& operator-=(Interval rhs);

    /// Equivalent to *this = factor * (*this)
    Interval& operator*=(value_t scale_factor);

    /// Equivalent to *this = (*this) / dividend
    Interval& operator/=(value_t divisor);

    /**
     * @brief Recieves two intervals \f$[a, b]\f$ and \f$[c, d]\f$ and computes
     * the interval \f$[a + c, b + d]\f$.
     */
    friend Interval operator+(Interval lhs, Interval rhs);

    /**
     * @brief Recieves two intervals \f$[a, b]\f$ and \f$[c, d]\f$ and computes
     * the interval \f$[a - c, b - d]\f$.
     */
    friend Interval operator-(Interval lhs, Interval rhs);

    /**
     * @brief Recieves a scaling factor \f$n\f$ and an interval \f$[a, b]\f$ and
     * computes the interval \f$[n * a, n * b]\f$.
     */
    friend Interval operator*(value_t scale_factor, Interval rhs);

    /**
     * @copydoc operator*(value_t, const Interval&)
     */
    friend Interval operator*(Interval lhs, value_t scale_factor);

    /**
     * @brief Recieves a divisor \f$n\f$ and an interval \f$[a, b]\f$ and
     * computes the interval \f$[a / n, b / n]\f$.
     *
     * @note The behaviour is undefined if \f$n\f$ is zero.
     */
    friend Interval operator/(Interval lhs, value_t inv_scale_factor);

    /**
     * @brief Calls `Interval::update(lhs, rhs, true)`.
     *
     * @see update(Interval&, const Interval&, bool)
     */
    friend bool update(Interval& lhs, Interval rhs);

    /**
     * @brief Intersects two intervals and assigns the result to the left
     * operand.
     *
     * @warning It is undefined behaviour to call this function with two
     * non-intersecting intervals.
     *
     * @returns \b true if the lower bound of the left operand changed by more
     * than g_epsilon or if check_upper is true and the greater
     * bound of the left operand changed by more than epsilon. \b false
     * otherwise.
     */
    friend bool update(Interval& lhs, Interval rhs, bool check_upper);

    /// Returns the length \f$b - a\f$ of the interval.
    double length() const
    {
        // Handles infinities!
        if (upper == lower) {
            return 0.0;
        }
        return upper - lower;
    }

    /**
     * @brief Checks if the bounds of the interval are approximately equal with
     * respect to the given tolerance.
     */
    bool bounds_approximately_equal(value_t epsilon = g_epsilon) const
    {
        return is_approx_equal(lower, upper, epsilon);
    }

    /// Stream output operator.
    friend std::ostream& operator<<(std::ostream&, Interval);
};

template <bool UseInterval>
using IncumbentSolution = std::conditional_t<UseInterval, Interval, value_t>;

/// Approximately compares the upper bounds of the intervals.
int approx_compare(Interval lhs, Interval rhs, value_t epsilon = g_epsilon);

/**
 * @brief Compares two state values approximately.
 *
 * @returns 0 if the values are approximately equal with respect to the
 * given epsilon, otherwise 1 if @p v2 is larger than @p v1 and otherwise -1.
 */
int approx_compare(value_t v1, value_t v2, value_t epsilon = g_epsilon);

void set_min(Interval& new_value, Interval tval);
void set_min(value_t& new_value, value_t tval);

// Value update
bool update(value_t& lhs, value_t rhs);
bool update(Interval& lhs, Interval rhs);
bool update(Interval& lhs, Interval rhs, bool check_upper);

value_t as_lower_bound(value_t single);
value_t as_lower_bound(Interval interval);

value_t as_upper_bound(value_t single);
value_t as_upper_bound(Interval interval);

} // namespace probfd

#endif // __VALUE_UTILS_H__