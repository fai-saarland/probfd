#pragma once

#include "value_type.h"

#include <cassert>
#include <unordered_map>

namespace probabilistic {

template<typename T>
class Distribution {
private:
    using distribution_t = typename std::unordered_map<T, value_type::value_t>;
    distribution_t distribution_;

public:
    using const_iterator = typename distribution_t::const_iterator;

    Distribution() { }
    Distribution(const Distribution&) = default;
    Distribution(Distribution&&) = default;
    ~Distribution() = default;
    Distribution& operator=(const Distribution&) = default;
    Distribution& operator=(Distribution&&) = default;

    size_t size() const { return distribution_.size(); }

    void clear() { distribution_.clear(); }

    void swap(Distribution<T>& other)
    {
        other.distribution_.swap(distribution_);
    }

    void add(const T& t, value_type::value_t prob)
    {
        assert(prob > value_type::zero);
        auto it = distribution_.insert(std::make_pair(t, prob));
        if (!it.second) {
            it.first->second += prob;
        }
    }

    bool empty() const { return distribution_.empty(); }

    void normalize(const value_type::value_t& scale)
    {
#ifndef NDEBUG
        value_type::value_t mass = value_type::zero;
#endif
        for (auto it = distribution_.begin(); it != distribution_.end(); it++) {
            it->second *= scale;
#ifndef NDEBUG
            mass += it->second;
#endif
        }
        assert(empty() || value_type::approx_equal()(mass, value_type::one));
    }

    void normalize()
    {
        if (empty()) {
            return;
        }
        value_type::value_t sum = value_type::zero;
        for (auto it = begin(); it != end(); it++) {
            sum += it->second;
        }
        assert(value_type::approx_greater()(sum, value_type::zero));
        normalize(value_type::one / sum);
    }

    bool is_normalized() const
    {
        value_type::value_t mass = value_type::zero;
        for (auto it = distribution_.begin(); it != distribution_.end(); ++it) {
            mass += it->second;
        }
        return value_type::approx_equal()(mass, value_type::one);
    }

    const_iterator erase(const_iterator it) { return distribution_.erase(it); }

    const_iterator begin() const { return distribution_.begin(); }

    const_iterator end() const { return distribution_.end(); }
};

} // namespace probabilistic
