#ifndef UTILS_MATH_H
#define UTILS_MATH_H

#include <cassert>
#include <concepts>
#include <limits>

namespace downward::utils {

template <std::integral T>
extern bool is_product_within_limit(T factor1, T factor2)
{
    if constexpr (std::numeric_limits<T>::is_signed) {
        assert(factor1 >= 0);
        assert(factor2 >= 0);
    }

    return factor2 == 0 || factor1 <= std::numeric_limits<T>::max() / factor2;
}

/* Test if the product of two numbers is bounded by a third number.
   Safe against overflow. The caller must guarantee
   0 <= factor1, factor2 <= limit; failing this is an error. */
template <std::integral T, std::integral U, std::integral V>
extern bool is_product_within_limit(T factor1, U factor2, V limit)
{
    if constexpr (std::numeric_limits<T>::is_signed) {
        assert(factor1 >= 0);
    }

    if constexpr (std::numeric_limits<U>::is_signed) {
        assert(factor2 >= 0);
    }

    if constexpr (std::numeric_limits<V>::is_signed) {
        assert(limit >= 0);
    }

    using R = std::make_unsigned_t<std::common_type_t<U, T, V>>;

    return factor2 == 0 || static_cast<R>(factor1) <=
                               static_cast<R>(limit) / static_cast<R>(factor2);
}

/* Test if the product of two numbers falls between the given inclusive lower
   and upper bounds. Safe against overflow. The caller must guarantee
   lower_limit < 0 and upper_limit >= 0; failing this is an error. */
extern bool is_product_within_limits(
    int factor1,
    int factor2,
    int lower_limit,
    int upper_limit);
} // namespace downward::utils

#endif
