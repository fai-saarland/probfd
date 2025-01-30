#ifndef PROBFD_VALUE_UTILS_H
#define PROBFD_VALUE_UTILS_H

#include "probfd/value_type.h"

#include <format>
#include <iosfwd>
#include <vector>

namespace probfd {

/// Represents a closed interval over the extended reals as a pair of lower and
/// upper bound.
struct Interval {
    value_t lower; ///< The Lower bound of the interval
    value_t upper; ///< The upper bound of the interval

    /// Constructs an interval consisting of a single point p.
    explicit Interval(value_t val = 0_vt);

    /// Constructs an interval from a specified lower and upper bound.
    explicit Interval(value_t lb, value_t ub);

    /// Equivalent to *this = *this + rhs
    Interval& operator+=(Interval rhs);

    /// Equivalent to *this = factor * (*this)
    Interval& operator*=(value_t scale_factor);

    /// Returns the length of the interval. If both bounds are infinity and
    /// have the same sign, returns 0.
    [[nodiscard]]
    double length() const;

    /**
     * @brief Checks if the length is below a given tolerance.
     *
     * @see Interval::length()
     */
    [[nodiscard]]
    bool bounds_approximately_equal(value_t tolerance) const;
};

/**
 * @brief Computes the component-wise addition of two intervals.
 *
 * @param lhs - An interval \f$[a_1, b_1]\f$
 * @param rhs - An interval \f$[a_2, b_2]\f$
 * @return The interval \f$[a_1 + a_2, b_1 + b_2]\f$
 */
Interval operator+(Interval lhs, Interval rhs);

/**
 * @brief Scales an interval.
 *
 * @param scale_factor - The scaling factor \f$n\f$
 * @param val - An interval \f$[a, b]\f$
 * @return The interval \f$[n \cdot a, n \cdot b]\f$
 */
Interval operator*(value_t scale_factor, Interval val);

/**
 * @copydoc operator*(value_t, Interval)
 */
Interval operator*(Interval val, value_t scale_factor);

/// Stream output operator. Prints '[<val.lower>,<val.upper>]'.
std::ostream& operator<<(std::ostream& os, Interval val);

} // namespace probfd

template <>
struct std::formatter<probfd::Interval> {
    std::formatter<std::vector<probfd::value_t>> inheritted;
    std::formatter<std::pair<probfd::value_t, probfd::value_t>> inheritted2;

    constexpr formatter() { inheritted2.set_brackets("[", "]"); }

    constexpr auto parse(std::format_parse_context& ctx)
    {
        return inheritted.parse(ctx);
    }

    auto
    format(const probfd::Interval& interval, std::format_context& ctx) const
    {
        return inheritted2.format(
            std::make_pair(interval.lower, interval.upper),
            ctx);
    }
};

#endif // PROBFD_VALUE_UTILS_H