#pragma once

#include "../../../utils/hash.h"
#include "types.h"

class GlobalState;

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

namespace probabilistic {
namespace pdbs {

struct PatternTooLargeException : std::exception {
    const char* what() const noexcept override {
        return "Construction of PDB would exceed "
            "std::numeric_limits<int>::max()";
    }
};

struct AbstractState {
    int id;
    explicit AbstractState(int id)
        : id(id)
    {
    }
    bool operator==(const AbstractState& x) const { return id == x.id; }
    bool operator<(const AbstractState& c) const { return id < c.id; }
    bool operator>(const AbstractState& c) const { return id > c.id; }
    AbstractState operator+(const AbstractState& x) const
    {
        return AbstractState(x.id + id);
    }
    AbstractState operator-(const AbstractState& x) const
    {
        return AbstractState(id - x.id);
    }
    AbstractState& operator+=(const AbstractState& x)
    {
        id += x.id;
        return *this;
    }
    AbstractState& operator-=(const AbstractState& x)
    {
        id -= x.id;
        return *this;
    }
};

extern std::ostream& operator<<(std::ostream& out, const AbstractState& s);

class AbstractStateMapper {
public:
    AbstractStateMapper(Pattern pattern, const std::vector<int>& domains);
    ~AbstractStateMapper() = default;

    unsigned int size() const;

    const Pattern& get_pattern() const;
    const std::vector<int>& get_domains() const;

    AbstractState operator()(const GlobalState& state) const;
    AbstractState operator()(const std::vector<int>& state) const;
    AbstractState from_values(const std::vector<int>& values) const;
    AbstractState from_values_partial(
        const std::vector<int>& indices,
        const std::vector<int>& values) const;
    AbstractState from_values_partial(
        const std::vector<std::pair<int, int>>& sparse_values) const;
    AbstractState from_value_partial(int idx, int val) const;

    int get_unique_partial_state_id(
        const std::vector<int>& indices,
        const std::vector<int>& values) const;

    std::vector<int> to_values(AbstractState abstract_state) const;

    void
    to_values(AbstractState abstract_state, std::vector<int>& values) const;

    template<typename Callback>
    void enumerate(
        std::vector<int>& values,
        const std::vector<int>& indices,
        Callback callback) const;

    template<typename Callback, typename... Args>
    void for_each_partial_state(
        AbstractState partial_offset,
        const std::vector<int>& indices,
        Callback callback,
        Args&&... args) const;

private:
    template<typename Callback>
    void enumerate(
        std::vector<int>& values,
        const std::vector<int>& indices,
        const Callback& callback,
        unsigned i) const;

    template<typename Callback, typename... Args>
    void for_each_partial_state(
        const std::vector<int>& multipliers,
        const std::vector<int>& domains,
        AbstractState base,
        unsigned i,
        const Callback& callback,
        Args&&... args) const;

    unsigned size_;
    std::vector<int> vars_;
    std::vector<int> domains_;
    std::vector<int> multipliers_;
    std::vector<int> partial_multipliers_;
};

class AbstractStateToString {
public:
    explicit AbstractStateToString(
        std::shared_ptr<AbstractStateMapper> state_mapper);
    std::string operator()(AbstractState state) const;

private:
    std::shared_ptr<AbstractStateMapper> state_mapper_;
};

template<typename Callback>
void
AbstractStateMapper::enumerate(
    std::vector<int>& values,
    const std::vector<int>& indices,
    Callback callback) const
{
    enumerate(values, indices, callback, 0);
}

template<typename Callback, typename... Args>
void
AbstractStateMapper::for_each_partial_state(
    AbstractState partial_offset,
    const std::vector<int>& indices,
    Callback callback,
    Args&&... args) const
{
    std::vector<int> fetched_multipliers(indices.size());
    std::vector<int> fetched_domains(indices.size());

    for (size_t i = 0; i != indices.size(); ++i) {
        const int index = indices[i];
        fetched_multipliers[i] = multipliers_[index];
        fetched_domains[i] = domains_[index];
    }

    for_each_partial_state(
        fetched_multipliers,
        fetched_domains,
        partial_offset,
        0,
        callback,
        std::forward<Args>(args)...);
}

template<typename Callback>
void
AbstractStateMapper::enumerate(
    std::vector<int>& values,
    const std::vector<int>& indices,
    const Callback& callback,
    unsigned i) const
{
    if (i == indices.size()) {
        callback(values);
    } else {
        const int idx = indices[i];
        for (int val = 0; val < domains_[idx]; val++) {
            values[idx] = val;
            enumerate(values, indices, callback, i + 1);
        }
        values[idx] = -1;
    }
}

template<typename Callback, typename... Args>
void
AbstractStateMapper::for_each_partial_state(
    const std::vector<int>& multipliers,
    const std::vector<int>& domains,
    AbstractState base,
    unsigned i,
    const Callback& callback,
    Args&&... args) const
{
    if (i == multipliers.size()) {
        std::invoke(callback, std::forward<Args>(args)..., base);
    } else {
        const int mult = multipliers[i];
        for (int val = 0; val < domains[i]; val++) {
            for_each_partial_state(
                multipliers,
                domains,
                base,
                i + 1,
                callback,
                std::forward<Args>(args)...);
            base.id += mult;
        }
    }
}

} // namespace pdbs
} // namespace probabilistic

namespace std {
template<>
struct hash<probabilistic::pdbs::AbstractState> {
    size_t operator()(const probabilistic::pdbs::AbstractState& state) const
    {
        return hash<int>()(state.id);
    }
};
} // namespace std

namespace utils {
void feed(HashState& h, const probabilistic::pdbs::AbstractState& s);
} // namespace utils
