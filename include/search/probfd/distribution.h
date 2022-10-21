#ifndef MDPS_DISTRIBUTION_H
#define MDPS_DISTRIBUTION_H

#include "probfd/value_type.h"

#include "utils/iterators.h"
#include "utils/range_proxy.h"
#include "utils/rng.h"

#include <algorithm>
#include <cassert>
#include <vector>

namespace probfd {

/**
 * @brief A convenience class that wraps a list of element-probability
 * pairs.
 *
 * @tparam T - The element type.
 */
template <typename T>
class Distribution {
private:
    using distribution_t =
        typename std::vector<std::pair<T, value_type::value_t>>;
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
    void add(T t, value_type::value_t prob)
    {
        assert(prob > 0.0);
        distribution_.emplace_back(std::move(t), prob);
    }

    std::pair<iterator, bool> add_unique(T t, value_type::value_t prob)
    {
        assert(prob > 0.0);

        auto it = this->find(t);

        if (it != end()) {
            it->second += prob;
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
    std::pair<T, value_type::value_t>&
    emplace(std::tuple<Args...> args, value_type::value_t prob)
    {
        assert(prob > 0.0);
        return distribution_.emplace_back(
            std::piecewise_construct,
            args,
            std::forward_as_tuple(prob));
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
     * @brief Scales all element probablities by a common factor.
     */
    void normalize(const value_type::value_t& scale)
    {
        for (auto it = distribution_.begin(); it != distribution_.end(); it++) {
            it->second *= scale;
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
        value_type::value_t sum = 0;
        for (auto it = begin(); it != end(); it++) {
            sum += it->second;
        }
        normalize(value_type::value_t(1.0) / sum);
    }

    /**
     * @brief Merges equal elements in the list by adding their probabilities.
     */
    void make_unique()
    {
        if (empty()) {
            return;
        }

        std::sort(distribution_.begin(), distribution_.end());
        unsigned i = 0;
        for (unsigned j = 1; j < distribution_.size(); ++j) {
            if (distribution_[i].first == distribution_[j].first) {
                distribution_[i].second += distribution_[j].second;
                distribution_[j].second = 0;
            } else {
                i = j;
            }
        }
        i = 1;
        for (unsigned j = 1; j < distribution_.size(); ++j) {
            if (distribution_[j].second != 0) {
                distribution_[i] = distribution_[j];
                ++i;
            }
        }
        distribution_.erase(distribution_.begin() + i, distribution_.end());
    }

    auto sample(utils::RandomNumberGenerator& rng)
    {
        assert(!empty());

        // Important!
        normalize();

        const value_type::value_t r = rng();

        auto it = distribution_.begin();
        value_type::value_t sum = it->second;

        while (sum <= r) {
            sum += (++it)->second;
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

    auto elem_begin() { return utils::make_key_iterator(begin()); }

    auto elem_begin() const { return utils::make_key_iterator(begin()); }

    auto elem_end() { return utils::make_key_iterator(end()); }

    auto elem_end() const { return utils::make_key_iterator(end()); }

    auto elements() { return utils::make_range(elem_begin(), elem_end()); }

    auto elements() const
    {
        return utils::make_range(elem_begin(), elem_end());
    }

    auto prob_begin() { return utils::make_val_iterator(begin()); }

    auto prob_begin() const { return utils::make_val_iterator(begin()); }

    auto prob_end() { return utils::make_val_iterator(end()); }

    auto prob_end() const { return utils::make_val_iterator(end()); }

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
};

} // namespace probfd

#endif // __DISTRIBUTION_H__