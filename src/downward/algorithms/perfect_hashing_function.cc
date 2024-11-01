#include "downward/algorithms/perfect_hashing_function.h"

#include <cassert>
#include <limits>

namespace perfect_hashing {

PerfectHashingFunction::PerfectHashingFunction(const std::vector<int>& domains)
    : safe_(true)
    , multiplyers_(domains.size(), 1)
    , offset_(0)
{
    const std::size_t x = std::numeric_limits<std::size_t>::max();
    for (unsigned i = 1; i < domains.size(); ++i) {
        if (x / domains[i - 1] <= multiplyers_[i - 1]) {
            safe_ = false;
            break;
        }
        multiplyers_[i] = multiplyers_[i - 1] * domains[i - 1];
    }
    if (safe_) {
        if (x / domains.back() <= multiplyers_.back()) {
            safe_ = false;
        } else {
            max_hash_ = multiplyers_.back() * domains.back();
        }
    }
}

PerfectHashingFunction::PerfectHashingFunction(
    const std::vector<int>& variables,
    const std::vector<int>& domains)
    : PerfectHashingFunction([&variables, &domains]() {
        std::vector<int> doms(variables.size(), 0);
        for (int i = variables.size() - 1; i >= 0; --i) {
            doms[i] = domains[variables[i]];
        }
        return doms;
    }())
{
}

bool PerfectHashingFunction::is_within_limits() const
{
    return safe_;
}

std::size_t PerfectHashingFunction::hash(const std::vector<int>& values) const
{
    assert(safe_);
    assert(values.size() == multiplyers_.size());
    std::size_t result = offset_;
    for (int i = values.size() - 1; i >= 0; i--) {
        result += values[i] * multiplyers_[i];
    }
    return result;
}

// std::size_t
// PerfectHashingFunction::hash(
//     const std::vector<int>& variables,
//     const std::vector<int>& values) const
// {
//     assert(safe_);
//     assert(variables.size() == multiplyers_.size());
//     std::size_t result = offset_;
//     for (int i = variables.size() - 1; i >= 0; --i) {
//         assert(variables[i] < static_cast<int>(values.size()));
//         const std::size_t value = values[variables[i]];
//         result += value * multiplyers_[i];
//     }
//     return result;
// }

void PerfectHashingFunction::hash_to_values(
    std::size_t hash,
    const std::vector<int>& variables,
    const std::vector<int>& domains,
    std::vector<int>& values) const
{
    hash -= offset_;
    values.resize(multiplyers_.size());
    for (int i = multiplyers_.size() - 1; i >= 0; --i) {
        values[i] =
            static_cast<int>(hash / multiplyers_[i]) % domains[variables[i]];
    }
}

std::size_t PerfectHashingFunction::get_max_hash() const
{
    assert(safe_);
    return max_hash_;
}

bool PerfectHashingFunction::set_offset(std::size_t offset)
{
    assert(safe_);
    offset_ = offset;
    if (std::numeric_limits<std::size_t>::max() - offset <= max_hash_) {
        safe_ = false;
        return false;
    }
    max_hash_ += offset;
    return true;
}

std::size_t PerfectHashingFunction::get_offset() const
{
    return offset_;
}

} // namespace perfect_hashing
