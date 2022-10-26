#pragma once

#include <algorithm>
#include <cassert>
#include <iterator>
#include <type_traits>
#include <vector>

namespace strips_utils {

using Fact = unsigned;
using FactSet = std::vector<Fact>;

template<typename... T>
using FactIndexedVector = std::vector<T...>;

namespace fact_set_internals {
class FactSetSetCounterBased;
} // namespace fact_set_internals

using FactSetSet = fact_set_internals::FactSetSetCounterBased;

unsigned size(const FactSet& fact_set);
bool insert(const Fact& fact, FactSet& fact_set);
bool remove(const Fact& fact, FactSet& fact_set);

namespace fact_set_internals {

class FactSetSetCounterBased {
public:
    explicit FactSetSetCounterBased(unsigned num_facts);
    ~FactSetSetCounterBased() = default;

    void set_num_facts(unsigned facts);

    unsigned size() const;
    bool contains(const FactSet& fact_set) const;
    unsigned get_id(const FactSet& fact_set) const;
    const FactSet& get(unsigned id) const;
    const FactSet& operator[](unsigned id) const;

    void clear();
    std::pair<unsigned, bool> insert(const FactSet& fact_set);

    template<typename F>
    void apply_to_subsets(const FactSet& fact_set, F fun) const;

    template<typename F>
    void apply_to_supersets(const FactSet& fact_set, F fun) const;

    std::vector<unsigned> get_subsets(const FactSet& fact_set) const;
    std::vector<unsigned>
    get_non_dominated_subsets(const FactSet& fact_set) const;

    bool contains_subset(const FactSet& fact_set) const;
    bool contains_superset(const FactSet& fact_set) const;

    std::vector<unsigned> get_supersets(const FactSet& fact_set) const;

private:
    template<typename F>
    static bool forward_call(const std::true_type&, F& fun, const unsigned& id);

    template<typename F>
    static bool
    forward_call(const std::false_type&, F& fun, const unsigned& id);

    unsigned num_facts_;
    mutable std::vector<unsigned> counter_;
    unsigned size_;
    int empty_fact_set_id_;
    std::vector<unsigned> sizes_;
    FactIndexedVector<std::vector<unsigned>> fact_to_fact_sets_;
    std::vector<FactSet> fact_sets_;
};

template<typename F>
bool
FactSetSetCounterBased::forward_call(
    const std::true_type&,
    F& fun,
    const unsigned& id)
{
    return fun(id);
}

template<typename F>
bool
FactSetSetCounterBased::forward_call(
    const std::false_type&,
    F& fun,
    const unsigned& id)
{
    fun(id);
    return false;
}

template<typename F>
void
FactSetSetCounterBased::apply_to_subsets(const FactSet& fact_set, F fun) const
{
    using return_type = typename std::invoke_result_t<F, unsigned>;
    auto early = std::is_convertible<return_type, bool>();
    std::fill(counter_.begin(), counter_.end(), 0);
    // if (empty_fact_set_id_ >= 0) {
    //     fun(empty_fact_set_id_);
    // }
    for (auto it = std::begin(fact_set); it != std::end(fact_set); ++it) {
        const auto& p = *it;
        const auto& refs = fact_to_fact_sets_[p];
        for (int i = refs.size() - 1; i >= 0; --i) {
            const unsigned id = refs[i];
            if (++counter_[id] == sizes_[id]) {
                assert(std::includes(
                    fact_set.begin(),
                    fact_set.end(),
                    fact_sets_[id].begin(),
                    fact_sets_[id].end()));
                if (forward_call(early, fun, id)) {
                    return;
                }
            }
        }
    }
}

template<typename F>
void
FactSetSetCounterBased::apply_to_supersets(const FactSet& fact_set, F fun) const
{
    using return_type = typename std::invoke_result_t<F, unsigned>;
    auto early = std::is_convertible<return_type, bool>();
    const unsigned size = strips_utils::size(fact_set);
    if (size == 0) {
        if (empty_fact_set_id_ >= 0) {
            fun(empty_fact_set_id_);
        }
        return;
    }
    std::fill(counter_.begin(), counter_.end(), 0);
    auto it = std::begin(fact_set);
    for (int j = size - 1; j >= 1; --j, ++it) {
        const auto& p = *it;
        const auto& refs = fact_to_fact_sets_[p];
        for (int i = refs.size() - 1; i >= 0; --i) {
            const unsigned id = refs[i];
            ++counter_[id];
        }
    }
    {
        assert(it != std::end(fact_set));
        const auto& p = *it;
        const auto& refs = fact_to_fact_sets_[p];
        for (int i = refs.size() - 1; i >= 0; --i) {
            const unsigned id = refs[i];
            if (++counter_[id] == size) {
                assert(std::includes(
                    fact_sets_[id].begin(),
                    fact_sets_[id].end(),
                    fact_set.begin(),
                    fact_set.end()));
                if (forward_call(early, fun, id)) {
                    return;
                }
            }
        }
        assert(++it == std::end(fact_set));
    }
}

} // namespace fact_set_internals
} // namespace strips_utils
