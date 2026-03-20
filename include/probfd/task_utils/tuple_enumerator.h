#ifndef PROBFD_TASK_UTILS_TUPLE_ENUMERATOR_H
#define PROBFD_TASK_UTILS_TUPLE_ENUMERATOR_H

#include "probfd/task_utils/tuple_enumerator_fwd.h"

#include <array>
#include <concepts>
#include <cstddef>
#include <utility>

namespace probfd::enumeration {

/**
 * @brief Represents an enumeration mapping for tuples of array indices.
 *
 * Given index sets \f$A_1, \dots A_n\f$ such that
 * \f$A_i = \{ 0, 1, \dots, C_i - 1 \}\f$,
 * this class represents a bijective mapping
 * \f[
 * f : \prod_{i = 1}^{n} A_i \to \{ 0, 1, \dots, \prod_{i = 1}^{n} C_i - 1 \}
 * \f]
 * as well as its inverse mapping.
 *
 * @tparam N The number of index sets.
 */
template <std::size_t N>
    requires(N > 0)
class TupleEnumerator {
    struct VariableInfo {
        int multiplier;
        int domain;
    };

    std::array<VariableInfo, N> infos;
    int product_size;

public:
    /// Constructs the object from the given cardinalities \f$C_i\f$ of the
    /// consecutive index sets involved in the Cartesian product.
    explicit TupleEnumerator(std::array<int, N> sizes)
        : TupleEnumerator(sizes, std::make_index_sequence<N>{})
    {
    }

    /// Constructs the object from the given cardinalities \f$C_i\f$ of the
    /// consecutive index sets involved in the Cartesian product.
    template <typename... T>
        requires(std::convertible_to<T, int> && ...) && (sizeof...(T) == N)
    explicit TupleEnumerator(T... sizes)
        : TupleEnumerator(
              std::array<int, N>{sizes...},
              std::make_index_sequence<N>{})
    {
    }

    /**
     * @brief Converts a tuple of indices to a unique index.
     *
     * This method has time complexity linear in N.
     */
    int to_index(std::array<int, N> state) const
    { return to_index(state, std::make_index_sequence<N>{}); }

    /**
     * @brief Converts a tuple of indices to a unique index.
     *
     * This method has time complexity linear in N.
     */
    template <typename... T>
        requires(std::convertible_to<T, int> && ...) && (sizeof...(T) == N)
    int to_index(T... local_state) const
    { return to_index(std::array<int, N>{local_state...}); }

    /**
     * @brief Computes the inverse of to_index .
     *
     * This method has time complexity linear in N.
     */
    std::array<int, N> to_tuple(int index) const
    { return to_tuple(index, std::make_index_sequence<N>{}); }

    /// Gets the cardinality \f$C_i\f$ of the I-th consecutive index set
    /// involved in the Cartesian product.
    template <std::size_t I>
        requires(I < N)
    int get_size() const
    { return infos[I].domain; }

    /**
     * @brief Gets the cardinality of the Cartesian product of the index sets,
     * i.e, the number \prod_{i = 1}^{n} C_i.
     *
     * This method has constant time complexity independent of N.
     */
    int get_product_size() const { return product_size; }

private:
    template <std::size_t... I>
    explicit TupleEnumerator(
        std::array<int, N> sizes,
        std::index_sequence<I...>)
    {
        int last_multiplier = 1;

        const auto work = [&last_multiplier](VariableInfo& info, int size) {
            info.domain = size;
            info.multiplier = last_multiplier;
            last_multiplier *= size;
        };

        (..., work(infos[I], sizes[I]));

        product_size = last_multiplier;
    }

    template <std::size_t... I>
    std::array<int, sizeof...(I)>
    to_tuple(int index, std::index_sequence<I...>) const
    { return {index / infos[I].multiplier % infos[I].domain...}; }

    template <std::size_t... I>
    int to_index(std::array<int, N> state, std::index_sequence<I...>) const
    { return (... + (infos[I].multiplier * state[I])); }
};

} // namespace probfd::enumeration

#endif // PROBFD_TASK_UTILS_TUPLE_ENUMERATOR_H
