#pragma once

#include <cassert>
#include <vector>

namespace perfect_hashing {

class PerfectHashingFunction {
public:
    explicit PerfectHashingFunction(const std::vector<int>& domains);
    explicit PerfectHashingFunction(
        const std::vector<int>& variables,
        const std::vector<int>& domains);
    ~PerfectHashingFunction() = default;

    bool is_within_limits() const;

    std::size_t hash(const std::vector<int>& values) const;
    template<typename V>
    std::size_t hash(const std::vector<int>& variables, const V& values) const;

    void hash_to_values(
        std::size_t hash,
        const std::vector<int>& variables,
        const std::vector<int>& domains,
        std::vector<int>& values) const;

    bool set_offset(std::size_t offset);
    std::size_t get_offset() const;

    std::size_t get_max_hash() const;

private:
    bool safe_;
    std::vector<std::size_t> multiplyers_;
    std::size_t max_hash_;
    std::size_t offset_;
};

template<typename V>
std::size_t
PerfectHashingFunction::hash(const std::vector<int>& variables, const V& values)
    const
{
    assert(safe_);
    assert(variables.size() == multiplyers_.size());
    std::size_t result = offset_;
    for (int i = variables.size() - 1; i >= 0; --i) {
        // assert(variables[i] < static_cast<int>(values.size()));
        const std::size_t value = values[variables[i]];
        result += value * multiplyers_[i];
    }
    return result;
}

} // namespace perfect_hashing
