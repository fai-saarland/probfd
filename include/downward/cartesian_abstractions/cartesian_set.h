#ifndef CEGAR_CARTESIAN_SET_H
#define CEGAR_CARTESIAN_SET_H

#include "downward/algorithms/dynamic_bitset.h"

#include <format>
#include <ostream>
#include <ranges>
#include <vector>

namespace downward::cartesian_abstractions {
using Bitset = dynamic_bitset::DynamicBitset<unsigned short>;

/*
  For each variable store a subset of its domain.

  The underlying data structure is a vector of bitsets.
*/
class CartesianSet {
    template <typename T, typename Char>
    friend struct std::formatter;

    std::vector<Bitset> domain_subsets;

public:
    explicit CartesianSet(const std::vector<int>& domain_sizes);

    void add(int var, int value);
    void set_single_value(int var, int value);
    void remove(int var, int value);
    void add_all(int var);
    void remove_all(int var);

    bool test(int var, int value) const { return domain_subsets[var][value]; }

    int count(int var) const;
    bool intersects(const CartesianSet& other, int var) const;
    bool is_superset_of(const CartesianSet& other) const;

    friend std::ostream&
    operator<<(std::ostream& os, const CartesianSet& cartesian_set);
};
} // namespace downward::cartesian_abstractions

template <typename Char>
struct std::formatter<downward::cartesian_abstractions::CartesianSet, Char> {
    using R = downward::cartesian_abstractions::Bitset;

    std::range_formatter<R, Char> underlying_;

    constexpr formatter()
    {
        underlying_.set_brackets("", "");
        underlying_.set_separator(" x ");
    }

    template <class ParseContext>
    constexpr typename ParseContext::iterator parse(ParseContext& ctx)
    {
        return underlying_.parse(ctx);
    }

    template <class FmtContext>
    typename FmtContext::iterator format(
        const downward::cartesian_abstractions::CartesianSet& t,
        FmtContext& ctx) const
    {        
        return underlying_.format(t.domain_subsets, ctx);
    }
};

#endif
