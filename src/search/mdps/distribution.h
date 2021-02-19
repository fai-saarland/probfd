#pragma once

#include "value_type.h"

#include <algorithm>
#include <cassert>
#include <vector>

namespace probabilistic {

template<typename T>
class Distribution {
private:
    using distribution_t =
        typename std::vector<std::pair<T, value_type::value_t>>;
    distribution_t distribution_;

public:
    using const_iterator = typename distribution_t::const_iterator;

    explicit Distribution() { }
    explicit Distribution(const Distribution&) = default;
    explicit Distribution(Distribution&&) = default;
    explicit Distribution(std::vector<std::pair<T, value_type::value_t>>&& d)
        : distribution_(std::move(d))
    {
    }
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
        assert(prob > 0.0);
        distribution_.emplace_back(t, prob);
    }

    bool empty() const { return distribution_.empty(); }

    void normalize(const value_type::value_t& scale)
    {
        for (auto it = distribution_.begin(); it != distribution_.end(); it++) {
            it->second *= scale;
        }
    }

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

    void make_unique()
    {
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
        distribution_.resize(i);
    }

    const_iterator erase(const_iterator it) { return distribution_.erase(it); }

    const_iterator begin() const { return distribution_.begin(); }

    const_iterator end() const { return distribution_.end(); }

    distribution_t& data() { return distribution_; } 

    const distribution_t& data() const { return distribution_; } 
};

} // namespace probabilistic
