#include "probfd/item_probability_pair.h"
#include "probfd/value_type.h"

#include "downward/utils/rng.h"

#include <algorithm>
#include <cassert>
#include <compare>
#include <functional>
#include <ranges>
#include <type_traits>
#include <utility>
#include <vector>

namespace probfd {

template <typename T>
Distribution<T>::Distribution() = default;

template <typename T>
Distribution<T>::Distribution(
    std::initializer_list<ItemProbabilityPair<T>> list)
    : distribution_(list)
{
    normalize();
}

template <typename T>
Distribution<T>::Distribution(
    std::initializer_list<ItemProbabilityPair<T>> list,
    NoNormalizeTagType)
    : distribution_(list)
{
}

template <typename T>
template <std::ranges::input_range R>
    requires(std::convertible_to<
             std::ranges::range_reference_t<R>,
             ItemProbabilityPair<T>>)
Distribution<T>::Distribution(std::from_range_t, R&& pair_range)
    : distribution_(std::from_range, std::forward<R>(pair_range))
{
    normalize();
}

template <typename T>
template <std::ranges::input_range R>
    requires(std::convertible_to<
             std::ranges::range_reference_t<R>,
             ItemProbabilityPair<T>>)
Distribution<T>::Distribution(
    std::from_range_t,
    NoNormalizeTagType,
    R&& pair_range)
#ifdef __cpp_lib_containers_ranges
    : distribution_(std::from_range, std::forward<R>(pair_range))
#else
    : distribution_(
          std::ranges::begin(pair_range),
          std::ranges::end(pair_range))
#endif
{
}

template <typename T>
void Distribution<T>::reserve(size_t capacity)
{
    distribution_.reserve(capacity);
}

template <typename T>
bool Distribution<T>::empty() const
{
    return distribution_.empty();
}

template <typename T>
size_t Distribution<T>::size() const
{
    return distribution_.size();
}

template <typename T>
void Distribution<T>::clear()
{
    distribution_.clear();
}

template <typename T>
void Distribution<T>::add_probability(T t, value_t prob)
{
    assert(prob > 0.0);

    auto it = this->find(t);

    if (it != end()) {
        it->probability += prob;
        return;
    }

    distribution_.emplace(it, std::move(t), prob);
}

template <typename T>
auto Distribution<T>::find(this auto&& self, const T& t)
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

template <typename T>
bool Distribution<T>::is_dirac(const T& t) const
{
    return size() == 1 && distribution_.front().item == t;
}

template <typename T>
bool Distribution<T>::is_dirac() const
{
    return size() == 1;
}

template <typename T>
template <InvocableRV<value_t, T> RandomVariable>
value_t Distribution<T>::expectation(RandomVariable&& rv) const
{
    value_t ex = 0_vt;
    for (const auto [succ, prob] : distribution_) {
        const value_t v = std::invoke(rv, succ);
        ex += prob * v;
    }
    return ex;
}

template <typename T>
template <SubscriptableRV<value_t, T> RandomVariable>
value_t Distribution<T>::expectation(RandomVariable&& rv) const
{
    value_t ex = 0_vt;
    for (const auto [succ, prob] : distribution_) {
        const value_t v = rv[succ];
        ex += prob * v;
    }
    return ex;
}

template <typename T>
void Distribution<T>::normalize(value_t scale)
{
    for (auto& pair : distribution_) {
        pair.probability *= scale;
    }
}

template <typename T>
void Distribution<T>::normalize()
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

template <typename T>
auto Distribution<T>::sample(utils::RandomNumberGenerator& rng)
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

template <typename T>
auto Distribution<T>::sample(utils::RandomNumberGenerator& rng) const
{
    assert(!empty());

    const value_t r = rng.random();

    auto it = distribution_.begin();
    value_t sum = it->probability;

    while (sum <= r) {
        sum += (++it)->probability;
    }

    return it;
}

template <typename T>
auto Distribution<T>::erase(iterator it) -> iterator
{
    return distribution_.erase(it);
}

template <typename T>
auto Distribution<T>::erase(iterator it, iterator last) -> iterator
{
    return distribution_.erase(it, last);
}

template <typename T>
template <std::predicate<const ItemProbabilityPair<T>&> P>
size_t Distribution<T>::remove_if(P&& pred)
{
    return std::erase_if(distribution_, std::forward<P>(pred));
}

template <typename T>
template <std::predicate<const ItemProbabilityPair<T>&> P>
value_t Distribution<T>::remove_if_normalize(P&& pred)
{
    value_t normalize_factor = 0_vt;

    std::erase_if(
        this->distribution_,
        [&pred, &normalize_factor](auto& target) {
            if (std::forward<P>(pred)(target)) {
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

template <typename T>
value_t Distribution<T>::remove_normalize(const T& t)
{
    return remove_if_normalize(
        [&](const auto& elem) { return elem.item == t; });
}

template <typename T>
auto Distribution<T>::begin(this auto&& self)
{
    return self.distribution_.begin();
}

template <typename T>
auto Distribution<T>::end(this auto&& self)
{
    return self.distribution_.end();
}

template <typename T>
auto Distribution<T>::support(this auto&& self)
{
    return std::views::transform(
        self.distribution_,
        &ItemProbabilityPair<T>::item);
}

template <typename T>
void swap(Distribution<T>& left, Distribution<T>& right)
{
    std::swap(left.distribution_, right.distribution_);
}

} // namespace probfd
