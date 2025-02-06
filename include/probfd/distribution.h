#ifndef PROBFD_DISTRIBUTION_H
#define PROBFD_DISTRIBUTION_H

#include "probfd/concepts.h"
#include "probfd/item_probability_pair.h"
#include "probfd/value_type.h"

#include "downward/utils/rng.h"

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

/// Disambiguator tag  for Distribution constructor to indicate that
/// the probabilities are already normalized to one.
inline constexpr NoNormalizeTagType no_normalize = NoNormalizeTagType{};

template <typename T>
class Distribution;

template <typename T>
void swap(Distribution<T>& left, Distribution<T>& right);

/**
 * @brief A convenience class that represents a finite probability
 * distribution.
 *
 * @tparam T - The item type.
 */
template <typename T>
class Distribution {
    std::vector<ItemProbabilityPair<T>> distribution_;

public:
    using iterator = typename std::vector<ItemProbabilityPair<T>>::iterator;
    using const_iterator =
        typename std::vector<ItemProbabilityPair<T>>::const_iterator;

    Distribution();

    Distribution(std::initializer_list<ItemProbabilityPair<T>> list);

    Distribution(
        std::initializer_list<ItemProbabilityPair<T>> list,
        NoNormalizeTagType);

    template <std::ranges::input_range R>
        requires(std::convertible_to<
                 std::ranges::range_reference_t<R>,
                 ItemProbabilityPair<T>>)
    explicit Distribution(std::from_range_t, R&& pair_range);

    template <std::ranges::input_range R>
        requires(std::convertible_to<
                 std::ranges::range_reference_t<R>,
                 ItemProbabilityPair<T>>)
    explicit Distribution(
        std::from_range_t,
        NoNormalizeTagType,
        R&& pair_range);

    /**
     * @brief Reserves space for \p capacity number of elements in
     * the support of the distribution.
     */
    void reserve(size_t capacity);

    /**
     * @brief Checks if the distribution is in an empty state.
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

    template <InvocableRV<value_t, T> RandomVariable>
    value_t expectation(RandomVariable&& rv) const;

    template <SubscriptableRV<value_t, T> RandomVariable>
    value_t expectation(RandomVariable&& rv) const;

    void normalize(value_t scale);
    void normalize();

    auto sample(utils::RandomNumberGenerator& rng);
    auto sample(utils::RandomNumberGenerator& rng) const;

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

    friend bool
    operator==(const Distribution<T>&, const Distribution<T>&) = default;

    friend auto
    operator<=>(const Distribution<T>&, const Distribution<T>&) = default;

    friend void swap<T>(Distribution<T>& left, Distribution<T>& right);
};

template <std::ranges::input_range R>
    requires(Specialization<std::ranges::range_value_t<R>, ItemProbabilityPair>)
Distribution(std::from_range_t, R&&)
    -> Distribution<std::tuple_element_t<0, std::ranges::range_value_t<R>>>;

} // namespace probfd

#include "probfd/distribution-impl.h"

#endif // PROBFD_DISTRIBUTION_H