#ifndef PROBFD_DISTRIBUTION_H
#define PROBFD_DISTRIBUTION_H

#include "probfd/value_type.h"

#include "utils/iterators.h"
#include "utils/rng.h"

#include <algorithm>
#include <cassert>
#include <compare>
#include <ranges>
#include <utility>
#include <vector>

namespace probfd {

/// An item-probability pair.
template <typename T>
class ItemProbabilityPair {
    template <typename... Args, size_t... Indices>
    ItemProbabilityPair(
        std::tuple<Args...>& args,
        value_t probability,
        std::index_sequence<Indices...>)
        : item(std::get<Indices>(std::move(args))...)
        , probability(probability)
    {
    }

public:
    T item;              ///< The item.
    value_t probability; ///< The probability of the item.

    /// Pairs a default-constructed item with an indeterminate probability.
    ItemProbabilityPair() = default;

    /// Pairs a given item with a given probability.
    ItemProbabilityPair(T item, value_t probability)
        : item(std::move(item))
        , probability(probability)
    {
    }

    /// Pairs an item constructed from a tuple of constructor arguments with a
    /// given probability.
    template <typename... Args>
    ItemProbabilityPair(
        std::tuple<Args...> constructor_args,
        value_t probability)
        : ItemProbabilityPair(
              constructor_args,
              probability,
              std::index_sequence_for<Args...>{})
    {
    }

    /// Comparison of two item-probability pairs.
    friend auto operator<=>(
        const ItemProbabilityPair<T>& left,
        const ItemProbabilityPair<T>& right) = default;
};

/**
 * @brief A convenience class that represents a finite probability distribution.
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

    /**
     * @brief Reserves space for \p capacity number of elements in
     * the support of the distribution.
     */
    void reserve(size_t capacity) { distribution_.reserve(capacity); }

    /**
     * @brief Checks if the distribution is in an empty state.
     */
    bool empty() const { return distribution_.empty(); }

    /**
     * @brief Returns the size of the support.
     */
    size_t size() const { return distribution_.size(); }

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

    iterator find(const T& t)
    {
        auto it = std::ranges::lower_bound(
            distribution_,
            t,
            std::less<>{},
            &ItemProbabilityPair<T>::item);

        if (it == end() || it->item == t) {
            return it;
        }

        return end();
    }

    const_iterator find(const T& t) const
    {
        auto it = std::ranges::lower_bound(
            distribution_,
            t,
            std::less<>{},
            &ItemProbabilityPair<T>::item);

        if (it == end() || it->item == t) {
            return it;
        }

        return end();
    }

    /**
     * @brief Checks if the distribution is a Dirac distribution wrt an element.
     */
    bool is_dirac(const T& t) const
    {
        return size() == 1 && distribution_.front().item == t;
    }

    /**
     * @brief Checks if the distribution is a Dirac distribution.
     */
    bool is_dirac() const { return size() == 1; }

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
    size_t remove_if_normalize(UnaryPredicate pred)
    {
        value_t normalize_factor = 0_vt;

        const auto num_removed = std::erase_if(
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

        return num_removed;
    }

    auto begin() { return distribution_.begin(); }

    auto begin() const { return distribution_.begin(); }

    auto end() { return distribution_.end(); }

    auto end() const { return distribution_.end(); }

    auto support()
    {
        return std::views::transform(
            distribution_,
            &ItemProbabilityPair<T>::item);
    }

    auto support() const
    {
        return std::views::transform(
            distribution_,
            &ItemProbabilityPair<T>::item);
    }

    friend auto
    operator<=>(const Distribution<T>&, const Distribution<T>&) = default;
};

} // namespace probfd

#endif // __DISTRIBUTION_H__