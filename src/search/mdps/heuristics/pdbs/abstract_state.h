#pragma once

#include "../../../utils/hash.h"

class GlobalState;

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace probabilistic {
namespace pdbs {

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
    AbstractStateMapper(
        const std::vector<int>& variables,
        const std::vector<int>& domains);
    ~AbstractStateMapper() = default;

    unsigned size() const;
    bool is_size_within_limits() const;
    const std::vector<int>& get_variables() const;
    const std::vector<int>& get_domains() const;

    AbstractState operator()(const GlobalState& state) const;
    AbstractState operator()(const std::vector<int>& state) const;
    AbstractState from_values(const std::vector<int>& values) const;
    AbstractState from_values_partial(
        const std::vector<int>& indices,
        const std::vector<int>& values) const;
    AbstractState from_value_partial(int idx, int val) const;

    std::vector<int> to_values(AbstractState abstract_state) const;

    void
    to_values(AbstractState abstract_state, std::vector<int>& values) const;

    template<typename Callback>
    void enumerate(
        const std::vector<int>& indices,
        std::vector<int>& values,
        Callback callback) const;

private:
    template<typename Callback>
    void enumerate(
        const std::vector<int>& indices,
        std::vector<int>& values,
        const Callback& callback,
        AbstractState base,
        unsigned i) const;

    bool oob_;
    unsigned size_;
    std::vector<int> vars_;
    std::vector<int> domains_;
    std::vector<int> multipliers_;
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
    const std::vector<int>& indices,
    std::vector<int>& values,
    Callback callback) const
{
    enumerate(indices, values, callback, AbstractState(0), 0);
}

template<typename Callback>
void
AbstractStateMapper::enumerate(
    const std::vector<int>& indices,
    std::vector<int>& values,
    const Callback& callback,
    AbstractState base,
    unsigned i) const
{
    if (i == indices.size()) {
        callback(base, values);
    } else {
        const int idx = indices[i];
        const int mult = multipliers_[idx];
        for (int val = 0; val < domains_[idx]; val++) {
            values[idx] = val;
            enumerate(indices, values, callback, base, i + 1);
            base.id += mult;
        }
        values[idx] = -1;
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
void
feed(HashState& h, const probabilistic::pdbs::AbstractState& s);
} // namespace utils
