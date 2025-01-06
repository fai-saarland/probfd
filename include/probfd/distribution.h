#ifndef PROBFD_DISTRIBUTION_H
#define PROBFD_DISTRIBUTION_H

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

/// An item-probability pair.
template <typename T, typename PrType = value_t>
class ItemProbabilityPair {
    template <
        typename... Args,
        typename... Args2,
        size_t... Indices,
        size_t... Indices2>
    ItemProbabilityPair(
        std::tuple<Args...> args,
        std::tuple<Args2...> args2,
        std::index_sequence<Indices...>,
        std::index_sequence<Indices2...>)
        : item(std::get<Indices>(args)...)
        , probability(std::get<Indices2>(args2)...)
    {
    }

public:
    T item;             ///< The item.
    PrType probability; ///< The probability of the item.

    /// Pairs a default-constructed item with an indeterminate probability.
    ItemProbabilityPair()
        requires(std::is_default_constructible_v<T> &&
                 std::is_default_constructible_v<PrType>)
    = default;

    template <typename A, typename B>
        requires(std::is_constructible_v<T, A> &&
                 std::is_constructible_v<PrType, B>)
    explicit(!std::is_convertible_v<T, A> || !std::is_convertible_v<PrType, B>)
        ItemProbabilityPair(std::pair<A, B> p)
        : item(std::get<0>(p))
        , probability(std::get<1>(p))
    {
    }

    template <typename A, typename B>
        requires(std::is_constructible_v<T, A> &&
                 std::is_constructible_v<PrType, B>)
    explicit(!std::is_convertible_v<T, A> || !std::is_convertible_v<PrType, B>)
        ItemProbabilityPair(std::tuple<A, B> p)
        : item(std::get<0>(p))
        , probability(std::get<1>(p))
    {
    }

    template <typename A>
        requires(std::is_constructible_v<T, A> &&
                 std::is_constructible_v<PrType, A>)
    explicit(!std::is_convertible_v<T, A> || !std::is_convertible_v<PrType, A>)
        ItemProbabilityPair(std::array<A, 2> p)
        : item(std::get<0>(p))
        , probability(std::get<1>(p))
    {
    }

    /// Pairs a given item with a given probability.
    template <typename A, typename B>
        requires(std::is_constructible_v<T, A> &&
                 std::is_constructible_v<PrType, B>)
    explicit(!std::is_convertible_v<T, A> || !std::is_convertible_v<PrType, B>)
        ItemProbabilityPair(A&& item, B&& probability)
        : item(std::forward<A>(item))
        , probability(std::forward<B>(probability))
    {
    }

    /// Pairs an item constructed from a tuple of constructor arguments with a
    /// given probability.
    template <typename... Args, typename... Args2>
    ItemProbabilityPair(
        std::piecewise_construct_t,
        std::tuple<Args...> t1,
        std::tuple<Args2...> t2)
        : ItemProbabilityPair(
              t1,
              t2,
              std::index_sequence_for<Args...>{},
              std::index_sequence_for<Args2...>{})
    {
    }

    /// Lexicographical comparison.
    friend auto operator<=>(
        const ItemProbabilityPair<T, PrType>& left,
        const ItemProbabilityPair<T, PrType>& right) = default;

    operator std::pair<T, PrType>() const
    {
        return std::make_pair(item, probability);
    }

    template <std::size_t Index>
    auto& get() &
    {
        if constexpr (Index == 0) return item;
        if constexpr (Index == 1) return probability;
    }

    template <std::size_t Index>
    const auto& get() const&
    {
        if constexpr (Index == 0) return item;
        if constexpr (Index == 1) return probability;
    }

    template <std::size_t Index>
    auto& get() &&
    {
        if constexpr (Index == 0) return std::move(item);
        if constexpr (Index == 1) return std::move(probability);
    }

    template <std::size_t Index>
    const auto& get() const&&
    {
        if constexpr (Index == 0) return std::move(item);
        if constexpr (Index == 1) return std::move(probability);
    }
};

/// Disambiguator tag type.
struct no_normalize_t {};

/// Disambiguator tag  for Distribution constructor to indicate that
/// the probabilities are already normalized to one.
inline constexpr no_normalize_t no_normalize = no_normalize_t{};

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

    Distribution() = default;

    Distribution(std::initializer_list<ItemProbabilityPair<T>> list)
        : distribution_(list)
    {
        normalize();
    }

    Distribution(
        std::initializer_list<ItemProbabilityPair<T>> list,
        no_normalize_t)
        : distribution_(list)
    {
    }

    template <std::ranges::input_range R>
        requires(std::convertible_to<
                 std::ranges::range_reference_t<R>,
                 ItemProbabilityPair<T>>)
    explicit Distribution(std::from_range_t, R&& pair_range)
        : distribution_(std::from_range, std::forward<R>(pair_range))
    {
        normalize();
    }

    template <std::ranges::input_range R>
        requires(std::convertible_to<
                 std::ranges::range_reference_t<R>,
                 ItemProbabilityPair<T>>)
    explicit Distribution(std::from_range_t, no_normalize_t, R&& pair_range)
        : distribution_(std::from_range, std::forward<R>(pair_range))
    {
    }

    /**
     * @brief Reserves space for \p capacity number of elements in
     * the support of the distribution.
     */
    void reserve(size_t capacity) { distribution_.reserve(capacity); }

    /**
     * @brief Checks if the distribution is in an empty state.
     */
    [[nodiscard]]
    bool empty() const
    {
        return distribution_.empty();
    }

    /**
     * @brief Returns the size of the support.
     */
    [[nodiscard]]
    size_t size() const
    {
        return distribution_.size();
    }

    void clear() { distribution_.clear(); }

    void swap(Distribution<T>& other)
    {
        other.distribution_.swap(distribution_);
    }

    void add_probability(T t, value_t prob)
    {
        assert(prob > 0.0);

        auto it = this->find(t);

        if (it != end()) {
            it->probability += prob;
            return;
        }

        distribution_.emplace(it, std::move(t), prob);
    }

    auto find(this auto&& self, const T& t)
    {
        auto it = std::ranges::lower_bound(
            self.distribution_,
            t,
            std::less<>{},
            &ItemProbabilityPair<T>::item);

        if (it == self.end() || it->item == t) {
            return it;
        }

        return self.end();
    }

    /**
     * @brief Checks if the distribution is a Dirac distribution wrt an
     * element.
     */
    bool is_dirac(const T& t) const
    {
        return size() == 1 && distribution_.front().item == t;
    }

    /**
     * @brief Checks if the distribution is a Dirac distribution.
     */
    [[nodiscard]]
    bool is_dirac() const
    {
        return size() == 1;
    }

    /**
     * @brief Computes the expectation over a real random variable according
     * to the distribution.
     */
    template <typename RandomVariable>
        requires(std::invocable<RandomVariable, T>)
    value_t expectation(RandomVariable rv) const
    {
        value_t ex = 0;
        for (const auto [succ, prob] : distribution_) {
            ex += prob * rv(succ);
        }
        return ex;
    }

    /**
     * @brief Computes the expectation over a real random variable according
     * to the distribution.
     */
    template <typename RandomVariable>
        requires requires(RandomVariable& rv, const T& t) {
            { rv[t] } -> std::convertible_to<value_t>;
        }
    value_t expectation(RandomVariable rv) const
    {
        value_t ex = 0_vt;
        for (const auto [succ, prob] : distribution_) {
            ex += prob * rv[succ];
        }
        return ex;
    }

    /**
     * @brief Scales all element probablities by a common factor.
     */
    void normalize(value_t scale)
    {
        for (auto& pair : distribution_) {
            pair.probability *= scale;
        }
    }

    /**
     * @brief Normalizes the probabilities of the elements to sum up to one.
     */
    void normalize()
    {
        if (empty()) {
            return;
        }
        value_t sum = 0;
        for (auto& pair : distribution_) {
            sum += pair.probability;
        }
        normalize(1_vt / sum);
    }

    auto sample(utils::RandomNumberGenerator& rng)
    {
        assert(!empty());

        // Important!
        normalize();

        const value_t r = rng.random();

        auto it = distribution_.begin();
        value_t sum = it->probability;

        while (sum <= r) {
            sum += (++it)->probability;
        }

        return it;
    }

    auto sample(utils::RandomNumberGenerator& rng) const
    {
        return static_cast<const_iterator>(
            const_cast<Distribution<T>*>(this)->sample(rng));
    }

    /**
     * @brief Removes the element-probability pair pointed to by \p it.
     *
     * \returns An iterator to the element-probability pair following the
     * erased pair.
     */
    iterator erase(iterator it) { return distribution_.erase(it); }

    /**
     * @brief Removes a range of element-probability pairs.
     *
     * \returns An iterator to the element-probability pair following the
     * last erased pair.
     */
    iterator erase(iterator it, iterator last)
    {
        return distribution_.erase(it, last);
    }

    template <typename UnaryPredicate>
    size_t remove_if(UnaryPredicate pred)
    {
        return std::erase_if(distribution_, pred);
    }

    template <typename UnaryPredicate>
    value_t remove_if_normalize(UnaryPredicate pred)
    {
        value_t normalize_factor = 0_vt;

        std::erase_if(
            this->distribution_,
            [&pred, &normalize_factor](auto& target) {
                if (pred(target)) {
                    normalize_factor += target.probability;
                    return true;
                }
                return false;
            });

        if (normalize_factor > 0) {
            this->normalize(1_vt / (1_vt - normalize_factor));
        }

        return normalize_factor;
    }

    value_t remove_if_normalize(const T& t)
    {
        return remove_if_normalize(
            [&](const auto& elem) { return elem.item == t; });
    }

    auto begin(this auto&& self) { return self.distribution_.begin(); }
    auto end(this auto&& self) { return self.distribution_.end(); }

    auto support(this auto&& self)
    {
        return std::views::transform(
            self.distribution_,
            &ItemProbabilityPair<T>::item);
    }

    friend auto
    operator<=>(const Distribution<T>& left, const Distribution<T>& right) =
        default;
};

namespace detail {

template <typename T>
struct is_item_prob_pair : std::false_type {};

template <typename T>
struct is_item_prob_pair<ItemProbabilityPair<T>> : std::true_type {};

template <typename T>
constexpr bool is_item_prob_pair_v = is_item_prob_pair<T>::value;

template <typename T>
struct item {};

template <typename T>
struct item<ItemProbabilityPair<T>> {
    using type = T;
};

template <typename T>
using item_t = typename item<T>::type;

}

template <std::ranges::input_range R>
    requires(detail::is_item_prob_pair_v<std::ranges::range_value_t<R>>)
Distribution(std::from_range_t, R&&)
    -> Distribution<detail::item_t<std::ranges::range_value_t<R>>>;

} // namespace probfd

template <typename T, typename F>
struct std::tuple_size<probfd::ItemProbabilityPair<T, F>>
    : public integral_constant<std::size_t, 2> {};

template <std::size_t I, typename T, typename F>
struct std::tuple_element<I, probfd::ItemProbabilityPair<T, F>> {
    static_assert(false, "Invalid index");
};

template <typename T, typename F>
struct std::tuple_element<0, probfd::ItemProbabilityPair<T, F>> {
    using type = T;
};

template <typename T, typename F>
struct std::tuple_element<1, probfd::ItemProbabilityPair<T, F>> {
    using type = F;
};

#endif // PROBFD_DISTRIBUTION_H