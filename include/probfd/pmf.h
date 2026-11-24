#ifndef PROBFD_PMF_H
#define PROBFD_PMF_H

#include "probfd/concepts.h"
#include "probfd/item_probability_pair.h"
#include "probfd/value_type.h"

#include "downward/utils/rng.h"
#include "downward/views/convert.h"

#include <algorithm>
#include <cassert>
#include <compare>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>

namespace probfd {

/// Disambiguator tag type.
struct NoNormalizeTagType {};

/// Disambiguator tag for PMF constructor to indicate that
/// the probabilities are already normalized to one.
inline constexpr auto no_normalize = NoNormalizeTagType{};

template <typename T>
class PMF;

template <typename T>
void swap(PMF<T>& left, PMF<T>& right) noexcept;

/**
 * @brief Represents a probability mass function (PMF).
 *
 * @tparam T - The sample space type.
 */
template <typename T>
class PMF {
    std::vector<ItemProbabilityPair<T>> graph_;

public:
    using iterator = std::vector<ItemProbabilityPair<T>>::iterator;
    using const_iterator = std::vector<ItemProbabilityPair<T>>::const_iterator;

    PMF();

    PMF(std::initializer_list<ItemProbabilityPair<T>> list);

    PMF(std::initializer_list<ItemProbabilityPair<T>> list, NoNormalizeTagType);

    template <std::ranges::input_range R>
        requires(std::convertible_to<
                 std::ranges::range_reference_t<R>,
                 ItemProbabilityPair<T>>)
    PMF(std::from_range_t, R&& pair_range);

    template <std::ranges::input_range R>
        requires(std::convertible_to<
                 std::ranges::range_reference_t<R>,
                 ItemProbabilityPair<T>>)
    PMF(std::from_range_t, NoNormalizeTagType, R&& pair_range);

    /**
     * @brief Reserves space for \p capacity number of elements in
     * the support of the PMF.
     */
    void reserve(size_t capacity);

    /**
     * @brief Checks if the PMF is in an empty state.
     */
    bool empty() const;

    /**
     * @brief Returns the size of the support.
     */
    size_t size() const;

    void clear();

    void add_probability(T t, value_t prob);

    auto find(this auto&& self, const T& t);

    bool is_dirac(const T& t) const;
    bool is_dirac() const;

    template <downward::InvocableRV<value_t, T> RandomVariable>
    value_t expectation(RandomVariable&& rv) const;

    template <downward::SubscriptableRV<value_t, T> RandomVariable>
    value_t expectation(RandomVariable&& rv) const;

    void normalize(value_t scale);
    void normalize();

    auto sample(downward::utils::RandomNumberGenerator& rng);

    iterator erase(iterator it);
    iterator erase(iterator it, iterator last);

    template <std::predicate<const ItemProbabilityPair<T>&> P>
    size_t remove_if(P&& pred);

    template <std::predicate<const ItemProbabilityPair<T>&> P>
    value_t remove_if_normalize(P&& pred);

    value_t remove_normalize(const T& t);

    auto begin(this auto&& self);
    auto end(this auto&& self);

    auto support(this auto&& self);

    friend bool operator==(const PMF&, const PMF&) = default;

    friend auto operator<=>(const PMF&, const PMF&) = default;

    friend void swap<T>(PMF& left, PMF& right) noexcept;
};

template <std::ranges::input_range R>
    requires(downward::Specialization<
             std::ranges::range_value_t<R>,
             ItemProbabilityPair>)
PMF(std::from_range_t, R&&)
    -> PMF<std::tuple_element_t<0, std::ranges::range_value_t<R>>>;

} // namespace probfd

template <typename T, typename Char>
    requires std::formattable<T, Char>
struct std::formatter<probfd::PMF<T>, Char> {
    std::range_formatter<std::pair<T, probfd::value_t>, Char> underlying_;

    constexpr formatter()
    {
        underlying_.underlying().set_brackets("", "");
        underlying_.underlying().set_separator(" -> ");
        underlying_.set_brackets("{", "}");
        underlying_.set_separator(", ");
    }

    template <class ParseContext>
    static constexpr ParseContext::iterator parse(ParseContext& ctx)
    {
        return ctx.begin();
    }

    template <class FmtContext>
    FmtContext::iterator format(const probfd::PMF<T>& p, FmtContext& ctx) const
    {
        return underlying_.format(
            p | downward::views::convert<std::pair<T, probfd::value_t>>,
            ctx);
    }
};

#include "probfd/pmf-impl.h"

#endif