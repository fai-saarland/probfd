#ifndef CEGAR_CARTESIAN_SET_H
#define CEGAR_CARTESIAN_SET_H

#include "downward/algorithms/dynamic_bitset.h"

#include <concepts>
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
    template <std::ranges::input_range R = std::initializer_list<int>>
        requires std::convertible_to<std::ranges::range_value_t<R>, int>
    explicit CartesianSet(R&& domain_sizes)
    {
        domain_subsets.reserve(domain_sizes.size());
        for (const int domain_size : domain_sizes) {
            domain_subsets.emplace_back(Bitset::ones(domain_size));
        }
    }

    template <
        std::ranges::input_range R = std::initializer_list<int>,
        std::ranges::input_range R2 =
            std::initializer_list<std::initializer_list<std::size_t>>>
        requires std::convertible_to<std::ranges::range_value_t<R>, int> &&
                 std::ranges::input_range<std::ranges::range_value_t<R2>> &&
                 std::convertible_to<
                     std::ranges::range_reference_t<
                         std::ranges::range_value_t<R2>>,
                     std::size_t>
    explicit CartesianSet(R&& domain_sizes, R2&& subsets)
    {
        domain_subsets.reserve(domain_sizes.size());
        for (const auto& [domain_size, subset] :
             std::views::zip(domain_sizes, subsets)) {
            domain_subsets.emplace_back(domain_size, subset);
        }
    }

    void add(int var, int value);
    void set_single_value(int var, int value);
    void remove(int var, int value);
    void add_all(int var);
    void remove_all(int var);

    bool test(int var, int value) const
    {
        return domain_subsets[var][value];
    }

    int count(int var) const;
    bool intersects(const CartesianSet& other, int var) const;
    bool is_superset_of(const CartesianSet& other) const;

    friend std::ostream&
    operator<<(std::ostream& os, const CartesianSet& cartesian_set);
};
} // namespace downward::cartesian_abstractions

template <typename Char>
struct std::formatter<downward::cartesian_abstractions::CartesianSet, Char> {
    using R =
        decltype(std::declval<const downward::cartesian_abstractions::Bitset&>()
                     .set_indices());

    std::range_formatter<R, Char> underlying_;

    constexpr formatter()
    {
        underlying_.set_brackets("", "");
        underlying_.set_separator(" x ");
        underlying_.underlying().set_brackets("{", "}");
    }

    template <class ParseContext>
    constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        if (*ctx.begin() != '}') {
            throw std::format_error("Expected '}'!");
        }
        return ctx.begin();
    }

    template <class FmtContext>
    FmtContext::iterator format(
        const downward::cartesian_abstractions::CartesianSet& t,
        FmtContext& ctx) const
    {
        auto r = t.domain_subsets |
                 std::views::transform(
                     &downward::cartesian_abstractions::Bitset::set_indices);
        return underlying_.format(r, ctx);
    }
};

#endif
