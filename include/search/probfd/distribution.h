#ifndef MDPS_DISTRIBUTION_H
#define MDPS_DISTRIBUTION_H

#include "probfd/value_type.h"

#include "utils/iterators.h"
#include "utils/range_proxy.h"
#include "utils/rng.h"

#include <algorithm>
#include <cassert>
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

    /// Compares two item-probability pairs lexigographically by operator<.
    friend bool operator<(
        const ItemProbabilityPair<T>& left,
        const ItemProbabilityPair<T>& right)
    {
        return std::tie(left.item, left.probability) <
               std::tie(right.item, right.probability);
    }

    /// Checks for equality of two item-probability pairs.
    friend bool operator==(
        const ItemProbabilityPair<T>& left,
        const ItemProbabilityPair<T>& right)
    {
        return std::tie(left.item, left.probability) ==
               std::tie(right.item, right.probability);
    }

    /// Compares the items of two item-probability pairs by operator<.
    static bool key_less(
        const ItemProbabilityPair<T>& left,
        const ItemProbabilityPair<T>& right)
    {
        return left.item < right.item;
    }

    /// Compares the items of two item-probability pairs by operator==.
    static bool key_equal(
        const ItemProbabilityPair<T>& left,
        const ItemProbabilityPair<T>& right)
    {
        return left.item == right.item;
    }
};

/**
 * @brief A convenience class that wraps a list of item-probability pairs.
 *
 * @tparam T - The item type.
 */
template <typename T>
class Distribution {
private:
    using distribution_t = std::vector<ItemProbabilityPair<T>>;
    distribution_t distribution_;

public:
    /// Iterator type.
    using iterator = typename distribution_t::iterator;
    using const_iterator = typename distribution_t::const_iterator;

    /// Constructs the empty list of element-probability pairs.
    Distribution() = default;

    /**
     * @brief Constructs the list from a vector of element-probability pairs.
     */
    explicit Distribution(distribution_t d)
        : distribution_(std::move(d))
    {
    }

    /**
     * @brief Returns the current number of element-probability pairs.
     */
    size_t size() const { return distribution_.size(); }

    /// @brief Clears the list.
    void clear() { distribution_.clear(); }

    void swap(Distribution<T>& other)
    {
        other.distribution_.swap(distribution_);
    }

    /**
     * Adds element-probability pair (\p t, \p prob) to the list,
     * even if another pair with an element that compares equal to \p t is
     * already present.
     *
     * @see make_unique
     */
    void add(T t, value_t prob)
    {
        assert(prob > 0.0);
        distribution_.emplace_back(std::move(t), prob);
    }

    std::pair<iterator, bool> add_unique(T t, value_t prob)
    {
        assert(prob > 0.0);

        auto it = this->find(t);

        if (it != end()) {
            it->probability += prob;
            return {it, false};
        }

        return {distribution_.emplace(it, std::move(t), prob), true};
    }

    /**
     * Emplaces element-probability pair (\p T(args...), \p prob) to the list,
     * even if another pair with an element that compares equal to \p t is
     * already present.
     *
     * @see make_unique
     */
    template <typename... Args>
    ItemProbabilityPair<T>& emplace(std::tuple<Args...> args, value_t prob)
    {
        assert(prob > 0.0);
        return distribution_.emplace_back(args, prob);
    }

    iterator find(const T& t)
    {
        return std::find(elem_begin(), elem_end(), t).base;
    }

    const_iterator find(const T& t) const
    {
        return std::find(elem_begin(), elem_end(), t).base;
    }

    /**
     * @brief Checks if the list is empty.
     */
    bool empty() const { return distribution_.empty(); }

    /**
     * @brief Checks if the distribution is a Dirac distribution wrt an element.
     */
    bool is_dirac(const T& t) const
    {
        return size() == 1 && *elem_begin() == t;
    }

    /**
     * @brief Checks if the distribution is a Dirac distribution.
     */
    bool is_dirac() const { return size() == 1; }

    /**
     * @brief Scales all element probablities by a common factor.
     */
    void normalize(const value_t& scale)
    {
        for (auto it = distribution_.begin(); it != distribution_.end(); it++) {
            it->probability *= scale;
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
        for (auto it = begin(); it != end(); it++) {
            sum += it->probability;
        }
        normalize(1_vt / sum);
    }

    /**
     * @brief Merges equal elements in the list by adding their probabilities.
     */
    void make_unique()
    {
        if (size() < 2) {
            return;
        }

        std::sort(
            distribution_.begin(),
            distribution_.end(),
            ItemProbabilityPair<T>::key_less);

        auto last = std::unique(
            distribution_.begin(),
            distribution_.end(),
            [](auto& left, auto& right) {
                if (left.item == right.item) {
                    left.probability += right.probability;
                    return true;
                }

                return false;
            });

        distribution_.erase(last, distribution_.end());
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
     * \returns An iterator to the element-probability pair directly behind the
     * erased pair.
     */
    iterator erase(iterator it) { return distribution_.erase(it); }

    auto begin() { return distribution_.begin(); }

    auto begin() const { return distribution_.begin(); }

    auto end() { return distribution_.end(); }

    auto end() const { return distribution_.end(); }

    auto elem_begin()
    {
        return utils::make_transform_iterator(
            begin(),
            &ItemProbabilityPair<T>::item);
    }

    auto elem_begin() const
    {
        return utils::make_transform_iterator(
            begin(),
            &ItemProbabilityPair<T>::item);
    }

    auto elem_end()
    {
        return utils::make_transform_iterator(
            end(),
            &ItemProbabilityPair<T>::item);
    }

    auto elem_end() const
    {
        return utils::make_transform_iterator(
            end(),
            &ItemProbabilityPair<T>::item);
    }

    auto elements() { return utils::make_range(elem_begin(), elem_end()); }

    auto elements() const
    {
        return utils::make_range(elem_begin(), elem_end());
    }

    auto prob_begin()
    {
        return utils::make_transform_iterator(
            begin(),
            &ItemProbabilityPair<T>::probability);
    }

    auto prob_begin() const
    {
        return utils::make_transform_iterator(
            begin(),
            &ItemProbabilityPair<T>::probability);
    }

    auto prob_end()
    {
        return utils::make_transform_iterator(
            end(),
            &ItemProbabilityPair<T>::probability);
    }

    auto prob_end() const
    {
        return utils::make_transform_iterator(
            end(),
            &ItemProbabilityPair<T>::probability);
    }

    auto probabilities() { return utils::make_range(prob_begin(), prob_end()); }

    auto probabilities() const
    {
        return utils::make_range(prob_begin(), prob_end());
    }

    /**
     * @brief Obtains a reference to the internal list.
     */
    distribution_t& data() { return distribution_; }

    /**
     * @brief Obtains a const reference to the internal list.
     */
    const distribution_t& data() const { return distribution_; }

    friend bool
    operator<(const Distribution<T>& left, const Distribution<T>& right)
    {
        return left.distribution_ < right.distribution_;
    }

    friend bool
    operator==(const Distribution<T>& left, const Distribution<T>& right)
    {
        return left.distribution_ == right.distribution_;
    }
};

} // namespace probfd

#endif // __DISTRIBUTION_H__