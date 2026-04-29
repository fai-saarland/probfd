#ifndef ALGORITHMS_DYNAMIC_BITSET_H
#define ALGORITHMS_DYNAMIC_BITSET_H

#include "downward/utils/string_literal.h"
#include "probfd/tasks/root_task.h"

#include <algorithm>
#include <cassert>
#include <compare>
#include <format>
#include <limits>
#include <numeric>
#include <ranges>
#include <tuple>
#include <vector>

/*
  Poor man's version of boost::dynamic_bitset, mostly copied from there.
*/

namespace downward::dynamic_bitset {

struct zero_construct_tag {};

struct one_construct_tag {};

inline constexpr auto construct_all_zeros = zero_construct_tag{};
inline constexpr auto construct_all_ones = one_construct_tag{};

template <std::unsigned_integral Block>
class DynamicBitset {
    template <typename T, typename Char>
    friend struct std::formatter;

    std::vector<Block> blocks;
    std::size_t num_bits;

    static constexpr Block ZEROS = Block(0);
    static constexpr Block ONES = Block(~Block(0));

    static constexpr std::size_t BITS_PER_BLOCK =
        std::numeric_limits<Block>::digits;

    static int compute_num_blocks(std::size_t num_bits)
    {
        return num_bits / BITS_PER_BLOCK +
               static_cast<int>(num_bits % BITS_PER_BLOCK != 0);
    }

    static std::pair<std::size_t, std::size_t> block_bit_index(std::size_t pos)
    { return {pos / BITS_PER_BLOCK, pos % BITS_PER_BLOCK}; }

    static Block bit_mask(std::size_t bit_index)
    { return Block(1) << bit_index; }

    std::size_t count_bits_in_last_block() const
    { return num_bits % BITS_PER_BLOCK; }

    void zero_unused_bits()
    {
        const std::size_t bits_in_last_block = count_bits_in_last_block();

        if (bits_in_last_block != 0) {
            assert(!blocks.empty());
            blocks.back() &= ~(ONES << bits_in_last_block);
        }
    }

public:
    class reference {
        typename std::vector<Block>::iterator block;
        Block bit_index;
        Block mask;

    public:
        reference(
            typename std::vector<Block>::iterator block,
            std::size_t bit_index)
            : block(block)
            , bit_index(bit_index)
            , mask(bit_mask(bit_index))
        {
        }

        operator bool() const { return (*block & mask) != 0; }

        bool operator~() const { return (*block & mask) == 0; }

        reference& operator=(bool b)
        {
            *block = (*block & ~mask) | (static_cast<Block>(b) << bit_index);
            return *this;
        }

        reference& flip()
        {
            *block ^= mask;
            return *this;
        }
    };

    class iterator {
        typename std::vector<Block>::iterator block;
        std::size_t bit_index;

    public:
        using value_type = reference;
        using difference_type = std::ptrdiff_t;

        iterator() = default;

        iterator(
            typename std::vector<Block>::iterator block,
            std::size_t bit_index)
            : block(block)
            , bit_index(bit_index)
        {
        }

        friend bool
        operator==(const iterator& lhs, const iterator& rhs) = default;

        friend bool
        operator<(const iterator& lhs, const iterator& rhs) = default;

        reference operator*() const { return reference(block, bit_index); }

        iterator& operator++()
        {
            if (++bit_index == BITS_PER_BLOCK) {
                bit_index = 0;
                ++block;
            }

            return *this;
        }

        iterator operator++(int)
        {
            iterator it = *this;
            ++(*this);
            return it;
        }
    };

    class const_iterator {
        typename std::vector<Block>::const_iterator block;
        std::size_t bit_index;

    public:
        using value_type = bool;
        using difference_type = std::ptrdiff_t;

        const_iterator() = default;

        const_iterator(
            typename std::vector<Block>::const_iterator block,
            std::size_t bit_index)
            : block(block)
            , bit_index(bit_index)
        {
        }

        friend bool
        operator==(const const_iterator& lhs, const const_iterator& rhs) =
            default;

        friend bool
        operator<(const const_iterator& lhs, const const_iterator& rhs) =
            default;

        bool operator*() const { return (*block & bit_mask(bit_index)) != 0; }

        const_iterator& operator++()
        {
            if (++bit_index == BITS_PER_BLOCK) {
                bit_index = 0;
                ++block;
            }

            return *this;
        }

        const_iterator operator++(int)
        {
            const_iterator it = *this;
            ++(*this);
            return it;
        }
    };

    explicit DynamicBitset(std::size_t num_bits)
        : blocks(compute_num_blocks(num_bits))
        , num_bits(num_bits)
    { zero_unused_bits(); }

    explicit DynamicBitset(std::size_t num_bits, zero_construct_tag)
        : blocks(compute_num_blocks(num_bits), ZEROS)
        , num_bits(num_bits)
    {
    }

    explicit DynamicBitset(std::size_t num_bits, one_construct_tag)
        : blocks(compute_num_blocks(num_bits), ONES)
        , num_bits(num_bits)
    { zero_unused_bits(); }

    template <std::ranges::input_range R>
        requires std::
            convertible_to<std::ranges::range_reference_t<R>, std::size_t>
        explicit DynamicBitset(std::size_t num_bits, R&& set_bits)
        : DynamicBitset(num_bits, construct_all_zeros)
    {
        for (const auto& i : set_bits) { set(static_cast<std::size_t>(i)); }
    }

    template <std::integral I>
    explicit DynamicBitset(
        std::size_t num_bits,
        std::initializer_list<I> set_bits)
        : DynamicBitset(num_bits, construct_all_zeros)
    {
        for (const std::integral auto i : set_bits) {
            set(static_cast<std::size_t>(i));
        }
    }

    template <std::ranges::input_range R>
        requires std::convertible_to<std::ranges::range_reference_t<R>, bool>
    explicit DynamicBitset(std::from_range_t, R&& range)
        : DynamicBitset(std::ranges::size(range))
    {
        const Block num_contiguous_blocks = num_bits / BITS_PER_BLOCK;
        const Block overhead_bits = num_bits % BITS_PER_BLOCK;

        auto bit = std::ranges::begin(blocks);
        const auto bend = bit + num_contiguous_blocks;

        auto it = std::ranges::begin(range);

        for (; bit != bend; ++bit) {
            Block block{};

            for (Block i{}; i != BITS_PER_BLOCK; ++i, ++it) {
                block |= static_cast<Block>(static_cast<bool>(*it)) << i;
            }

            *bit = block;
        }

        if (overhead_bits != 0) {
            Block block{};

            for (Block i{}; i != overhead_bits; ++i, ++it) {
                block |= static_cast<Block>(static_cast<bool>(*it)) << i;
            }

            *bit = block;
        }
    }

    static DynamicBitset zeros(std::size_t num_bits)
    { return DynamicBitset(num_bits, construct_all_zeros); }

    static DynamicBitset ones(std::size_t num_bits)
    { return DynamicBitset(num_bits, construct_all_ones); }

    std::size_t size() const { return num_bits; }

    /*
      Count the number of set bits.

      The computation could be made faster by using a more sophisticated
      algorithm (see https://en.wikipedia.org/wiki/Hamming_weight).
    */
    int count() const
    {
        auto pcs = blocks | std::views::transform(
                                [](Block b) { return std::popcount(b); });

        return std::accumulate(pcs.begin(), pcs.end(), 0);
    }

    void set()
    {
        std::ranges::fill(blocks, ONES);
        zero_unused_bits();
    }

    void reset() { std::ranges::fill(blocks, ZEROS); }

    void set(std::size_t pos)
    {
        assert(pos < num_bits);
        const auto [block_index, bit_index] = block_bit_index(pos);
        blocks[block_index] |= bit_mask(bit_index);
    }

    void reset(std::size_t pos)
    {
        assert(pos < num_bits);
        const auto [block_index, bit_index] = block_bit_index(pos);
        blocks[block_index] &= ~bit_mask(bit_index);
    }

    bool test(std::size_t pos) const
    {
        assert(pos < num_bits);
        const auto [block_index, bit_index] = block_bit_index(pos);
        return (blocks[block_index] & bit_mask(bit_index)) != 0;
    }

    reference operator[](std::size_t pos)
    {
        const auto [block_index, bit_index] = block_bit_index(pos);
        return reference(blocks.begin() + block_index, bit_index);
    }

    bool operator[](std::size_t pos) const { return test(pos); }

    bool intersects(const DynamicBitset& other) const
    {
        assert(size() == other.size());
        for (std::size_t i = 0; i < blocks.size(); ++i) {
            if (blocks[i] & other.blocks[i]) return true;
        }
        return false;
    }

    bool is_subset_of(const DynamicBitset& other) const
    {
        assert(size() == other.size());
        for (std::size_t i = 0; i < blocks.size(); ++i) {
            if (blocks[i] & ~other.blocks[i]) return false;
        }
        return true;
    }

    constexpr auto set_indices() const
    {
        return std::views::iota(0U, num_bits) |
               std::views::filter(
                   [&](std::integral auto i) { return test(i); });
    }

    friend auto
    operator<=>(const DynamicBitset& left, const DynamicBitset& right) =
        default;

    friend bool
    operator==(const DynamicBitset& left, const DynamicBitset& right) = default;

    auto begin() { return iterator(blocks.begin(), 0); }

    auto end()
    {
        const auto [block_index, bit_index] = block_bit_index(num_bits);
        return iterator(blocks.begin() + block_index, bit_index);
    }

    auto begin() const { return const_iterator(blocks.begin(), 0); }

    auto end() const
    {
        const auto [block_index, bit_index] = block_bit_index(num_bits);
        return const_iterator(blocks.begin() + block_index, bit_index);
    }
};

static_assert(std::ranges::sized_range<DynamicBitset<unsigned int>>);

} // namespace downward::dynamic_bitset

template <typename Block, typename Char>
struct std::formatter<downward::dynamic_bitset::DynamicBitset<Block>, Char> {
    std::formatter<unsigned int, Char> underlying_;

    constexpr formatter()
    {
        std::basic_format_parse_context<Char> ctx2("b}");
        underlying_.parse(ctx2);
    }

    template <class ParseContext>
    constexpr typename ParseContext::iterator parse(ParseContext& ctx)
    {
        if (*ctx.begin() != '}') { throw std::format_error("Expected '}'!"); }
        return ctx.begin();
    }

    template <class FmtContext>
    typename FmtContext::iterator format(
        const downward::dynamic_bitset::DynamicBitset<Block>& bs,
        FmtContext& ctx) const
    {
        const auto num_contiguous_blocks =
            bs.num_bits /
            downward::dynamic_bitset::DynamicBitset<Block>::BITS_PER_BLOCK;

        const auto overhead_bits =
            bs.num_bits %
            downward::dynamic_bitset::DynamicBitset<Block>::BITS_PER_BLOCK;

        auto bit = std::ranges::begin(bs.blocks);
        const auto bend = bit + num_contiguous_blocks;

        {
            constexpr Block l =
                downward::dynamic_bitset::DynamicBitset<Block>::BITS_PER_BLOCK;

            for (; bit != bend; ++bit) {
                Block b = *bit;
                for (Block i = 0; i != l; b >>= 1, ++i) {
                    ctx.advance_to(underlying_.format(b & 1, ctx));
                }
            }
        }

        if (overhead_bits != 0) {
            const Block l = static_cast<Block>(overhead_bits);

            Block b = *bit;

            for (Block i = 0; i != l; b >>= 1, ++i) {
                ctx.advance_to(underlying_.format(b & 1, ctx));
            }
        }

        return ctx.out();
    }
};

/*
This source file was derived from the boost::dynamic_bitset library
version 1.54. Original copyright statement and license for this
original source follow.

Copyright (c) 2001-2002 Chuck Allison and Jeremy Siek
Copyright (c) 2003-2006, 2008 Gennaro Prota

Distributed under the Boost Software License, Version 1.0.

Boost Software License - Version 1.0 - August 17th, 2003

Permission is hereby granted, free of charge, to any person or organization
obtaining a copy of the software and accompanying documentation covered by
this license (the "Software") to use, reproduce, display, distribute,
execute, and transmit the Software, and to prepare derivative works of the
Software, and to permit third-parties to whom the Software is furnished to
do so, all subject to the following:

The copyright notices in the Software and this entire statement, including
the above license grant, this restriction and the following disclaimer,
must be included in all copies of the Software, in whole or in part, and
all derivative works of the Software, unless such copies or derivative
works are solely in the form of machine-executable object code generated by
a source language processor.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE, TITLE AND NON-INFRINGEMENT. IN NO EVENT
SHALL THE COPYRIGHT HOLDERS OR ANYONE DISTRIBUTING THE SOFTWARE BE LIABLE
FOR ANY DAMAGES OR OTHER LIABILITY, WHETHER IN CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
DEALINGS IN THE SOFTWARE.
*/

#endif
