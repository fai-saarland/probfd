#include "fact_set.h"

#include <algorithm>
#include <cassert>

namespace strips_utils {

unsigned
size(const FactSet& fact_set)
{
    return fact_set.size();
}

bool
insert(const Fact& fact, FactSet& fact_set)
{
    auto it = std::lower_bound(fact_set.begin(), fact_set.end(), fact);
    if (it == fact_set.end() || (*it) != fact) {
        fact_set.insert(it, fact);
        return true;
    }
    return false;
}

bool
remove(const Fact& fact, FactSet& fact_set)
{
    auto it = std::lower_bound(fact_set.begin(), fact_set.end(), fact);
    if (it != fact_set.end() && *it == fact) {
        fact_set.erase(it);
        return true;
    }
    return false;
}

namespace fact_set_internals {

FactSetSetCounterBased::FactSetSetCounterBased(unsigned num_facts)
    : num_facts_(num_facts)
    , size_(0)
    , empty_fact_set_id_(-1)
    , fact_to_fact_sets_(num_facts)
{
}

void
FactSetSetCounterBased::set_num_facts(unsigned facts)
{
    fact_to_fact_sets_.resize(facts);
    num_facts_ = facts;
}

unsigned
FactSetSetCounterBased::size() const
{
    return size_;
}

unsigned
FactSetSetCounterBased::get_id(const FactSet& fact_set) const
{
    const unsigned size = strips_utils::size(fact_set);
    int result = -1;
    auto check_size = [this, &result, &size](unsigned id) {
        if (size == sizes_[id]) {
            result = id;
            return true;
        }
        return false;
    };
    apply_to_supersets(fact_set, check_size);
    return result;
}

bool
FactSetSetCounterBased::contains(const FactSet& fact_set) const
{
    return get_id(fact_set) < size_;
}

std::pair<unsigned, bool>
FactSetSetCounterBased::insert(const FactSet& fact_set)
{
#if !defined(NDEBUG)
    assert(std::is_sorted(fact_set.begin(), fact_set.end()));
    {
        FactSet copy(fact_set);
        assert(std::unique(copy.begin(), copy.end()) == copy.end());
    }
#endif
    unsigned id = get_id(fact_set);
    if (id < size_) {
        return std::pair<unsigned, bool>(id, false);
    }
    id = size_++;
    sizes_.push_back(strips_utils::size(fact_set));
    counter_.emplace_back(0);
    fact_sets_.push_back(fact_set);
    if (sizes_.back() == 0) {
        empty_fact_set_id_ = id;
    } else {
        for (auto it = std::begin(fact_set); it != std::end(fact_set); ++it) {
            fact_to_fact_sets_[*it].push_back(id);
        }
    }
    return std::pair<unsigned, bool>(id, true);
}

const FactSet&
FactSetSetCounterBased::get(unsigned id) const
{
    return fact_sets_[id];
}

const FactSet&
FactSetSetCounterBased::operator[](unsigned id) const
{
    return fact_sets_[id];
}

std::vector<unsigned>
FactSetSetCounterBased::get_supersets(const FactSet& fact_set) const
{
    std::vector<unsigned> result;
    auto store = [&result](unsigned id) { result.push_back(id); };
    apply_to_supersets(fact_set, store);
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<unsigned>
FactSetSetCounterBased::get_subsets(const FactSet& fact_set) const
{
    std::vector<unsigned> result;
    auto store = [&result](unsigned id) { result.push_back(id); };
    apply_to_subsets(fact_set, store);
    std::sort(result.begin(), result.end());
    return result;
}

bool
FactSetSetCounterBased::contains_subset(const FactSet& fact_set) const
{
    bool result = false;
    auto store = [&result](unsigned) {
        result = true;
        return true;
    };
    apply_to_subsets(fact_set, store);
    return result;
}

bool
FactSetSetCounterBased::contains_superset(const FactSet& fact_set) const
{
    bool result = false;
    auto store = [&result](unsigned) {
        result = true;
        return true;
    };
    apply_to_supersets(fact_set, store);
    return result;
}

std::vector<unsigned>
FactSetSetCounterBased::get_non_dominated_subsets(const FactSet& fact_set) const
{
    std::vector<unsigned> result = get_subsets(fact_set);
    unsigned j = 1;
    while (j < result.size()) {
        const auto& setj = fact_sets_[result[j]];
        bool dominated = false;
        for (int k = j - 1; k >= 0; k--) {
            const auto& setk = fact_sets_[result[k]];
            if (strips_utils::size(setj) < strips_utils::size(setk)) {
                if (std::includes(
                        setk.begin(), setk.end(), setj.begin(), setj.end())) {
                    dominated = true;
                    break;
                }
            } else if (strips_utils::size(setj) > strips_utils::size(setk)) {
                if (std::includes(
                        setj.begin(), setj.end(), setk.begin(), setk.end())) {
                    result.erase(result.begin() + k);
                    j--;
                }
            }
        }
        if (dominated) {
            result.erase(result.begin() + j);
        } else {
            j++;
        }
    }
    return result;
}

void
FactSetSetCounterBased::clear()
{
    size_ = 0;
    empty_fact_set_id_ = -1;
    counter_.clear();
    sizes_.clear();
    fact_sets_.clear();
    fact_to_fact_sets_ = FactIndexedVector<std::vector<unsigned>>(num_facts_);
}

} // namespace fact_set_internals
} // namespace strips_utils
