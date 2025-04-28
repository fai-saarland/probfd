#ifndef FACT_PAIR_H
#define FACT_PAIR_H

#include "downward/utils/hash.h"

#include "downward/concepts.h"

#include <compare>
#include <concepts>
#include <type_traits>
#include <iosfwd>
#include <utility>

namespace downward {
struct FactPair {
    int var;
    int value;

    FactPair(int var, int value)
        : var(var)
        , value(value)
    {
    }

    template <PairLike P>
        requires(std::convertible_to<std::tuple_element_t<0, P>, int> &&
                 std::convertible_to<std::tuple_element_t<1, P>, int>)
    explicit(false) FactPair(P pair)
        : var(std::get<0>(pair))
        , value(std::get<1>(pair))
    {
    }

    friend auto operator<=>(const FactPair&, const FactPair&) = default;

    /*
      This special object represents "no such fact". E.g., functions
      that search a fact can return "no_fact" when no matching fact is
      found.
    */
    static const FactPair no_fact;
};

std::ostream& operator<<(std::ostream& os, const FactPair& fact_pair);
} // namespace downward

namespace downward::utils {
inline void feed(HashState& hash_state, const FactPair& fact)
{
    feed(hash_state, fact.var);
    feed(hash_state, fact.value);
}
} // namespace downward::utils

#endif
