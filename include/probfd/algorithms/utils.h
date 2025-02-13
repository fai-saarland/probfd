#ifndef PROBFD_ALGORITHMS_UTILS_H
#define PROBFD_ALGORITHMS_UTILS_H

#include "probfd/value_type.h"

#include <cassert>
#include <cstddef>
#include <tuple>
#include <utility>

// Forward Declarations
namespace probfd {
struct Interval;
}

namespace probfd::algorithms {

template <typename T>
concept Clearable = requires (T& t) { t.clear(); };

/**
 * @brief Helper RAII class that ensures that containers are cleared when going
 * out of scope.
 */
template <Clearable... T>
class ClearGuard {
    std::tuple<T&...> containers_;

public:
    explicit ClearGuard(T&... containers)
        : containers_(containers...)
    {
        assert((containers.empty() && ...));
    }

    ~ClearGuard()
    {
        std::apply([](auto&... c) { (c.clear(), ...); }, containers_);
    }
};

/**
 * @brief Function object calling std::get<n> on its argument. Useful in ranges
 * algorithms.
 */
template <size_t n>
struct get_t {
    template <typename T>
    decltype(auto) operator()(T&& t) const
    {
        using std::get;
        return get<n>(std::forward<T>(t));
    }
};

template <size_t n>
inline constexpr get_t<n> project;

/// Returns the lower bound of the interval.
value_t as_lower_bound(Interval interval);

/// Returns the value unchanged.
value_t as_lower_bound(value_t single);

/// Returns the interval with the given lower bound and infinte upper bound.
Interval as_interval(value_t lower_bound);

/// Returns the value unchanged.
Interval as_interval(Interval value);

/**
 * @brief Computes the assignments `lhs.lower <- min(lhs.lower, rhs.lower)` and
 * `lower <- min(lhs.lower, rhs.lower)`.
 *
 * @returns True if and only if \p rhs.lower was less than \p lhs.lower .
 */
bool set_min(Interval& lhs, Interval rhs);

/**
 * @brief Computes the assignment lhs <- min(lhs, rhs).
 *
 * @returns True if and only if \p rhs was less than \p lhs .
 */
bool set_min(value_t& lhs, value_t rhs);

/**
 * @brief Intersects two intervals and assigns the result to the left
 * operand.
 *
 * @returns \b true if the lower or upper bound of the left operand changed by
 * more than g_epsilon, \b false otherwise.
 *
 * @warning It is undefined behaviour to call this function with two
 * non-intersecting intervals.
 */
bool update(Interval& lhs, Interval rhs, value_t epsilon);

// Value update
bool update(value_t& lhs, value_t rhs, value_t epsilon);

} // namespace probfd::algorithms

#endif