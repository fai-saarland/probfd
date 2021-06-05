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
    class CartesianSubsetIterator;
    class CartesianSubsetEndIterator;

    class PartialStateIterator;
    class PartialStateEndIterator;

    class CartesianSubsetIterator {
        friend AbstractStateMapper;

        using reference = std::vector<int>&;
        using pointer = std::vector<int>*;

        std::vector<int> values_;
        std::vector<int> indices_;

        const std::vector<int>& multipliers_;
        const std::vector<int>& domains_;

        int i;

    public:
        CartesianSubsetIterator(
            std::vector<int> values,
            std::vector<int> indices,
            const std::vector<int>& multipliers,
            const std::vector<int>& domains);

        CartesianSubsetIterator& operator++();
        CartesianSubsetIterator& operator--();

        CartesianSubsetIterator operator++(int) = delete;
        CartesianSubsetIterator operator--(int) = delete;

        reference operator*();
        pointer operator->();

        friend bool operator==(
            const CartesianSubsetIterator&,
            const CartesianSubsetEndIterator&);

        friend bool operator!=(
            const CartesianSubsetIterator&,
            const CartesianSubsetEndIterator&);
    };

    class CartesianSubsetEndIterator {
        friend AbstractStateMapper;

        friend bool operator==(
            const CartesianSubsetIterator&,
            const CartesianSubsetEndIterator&);

        friend bool operator!=(
            const CartesianSubsetIterator&,
            const CartesianSubsetEndIterator&);
    };

    class PartialStateIterator {
        friend AbstractStateMapper;

        AbstractState state_;

        std::vector<int> indices_;

        std::vector<int> index_multipliers_;
        std::vector<int> iterations_left_;

        int index = 0;

    public:
        PartialStateIterator(
            AbstractState state,
            std::vector<int> indices,
            const std::vector<int>& multipliers,
            const std::vector<int>& domains);

        PartialStateIterator& operator++();
        PartialStateIterator& operator--() = delete;

        PartialStateIterator operator++(int) = delete;
        PartialStateIterator operator--(int) = delete;

        AbstractState operator*();
        AbstractState* operator->();

        friend bool operator==(
            const PartialStateIterator&,
            const PartialStateEndIterator&);

        friend bool operator!=(
            const PartialStateIterator&,
            const PartialStateEndIterator&);
    };

    class PartialStateEndIterator {
        friend AbstractStateMapper;

        friend bool operator==(
            const PartialStateIterator&,
            const PartialStateEndIterator&);

        friend bool operator!=(
            const PartialStateIterator&,
            const PartialStateEndIterator&);
    };

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

    CartesianSubsetIterator cartesian_subsets_begin(
        std::vector<int> values,
        std::vector<int> indices) const;

    CartesianSubsetEndIterator cartesian_subsets_end() const;

    PartialStateIterator partial_states_begin(
        AbstractState offset,
        std::vector<int> indices) const;

    PartialStateEndIterator partial_states_end() const;

private:
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
