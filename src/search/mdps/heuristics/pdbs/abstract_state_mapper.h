#pragma once

#include "abstract_state.h"
#include "types.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

class GlobalState;

namespace probabilistic {
namespace pdbs {

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

        const std::vector<int>& domains_;

        int i;

    public:
        CartesianSubsetIterator(
            std::vector<int> values,
            std::vector<int> indices,
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

        std::vector<int> index_multipliers_;
        std::vector<int> local_counters_;
        std::vector<AbstractState> argument_states_;
        std::vector<int> domains_;

        int recursion_level;

    public:
        PartialStateIterator(
            AbstractState state,
            const std::vector<int>& indices,
            const std::vector<int>& multipliers,
            const std::vector<int>& domains);

        PartialStateIterator& operator++();
        PartialStateIterator& operator--() = delete;

        PartialStateIterator operator++(int) = delete;
        PartialStateIterator operator--(int) = delete;

        AbstractState operator*();
        AbstractState* operator->();

        friend bool
        operator==(const PartialStateIterator&, const PartialStateEndIterator&);

        friend bool
        operator!=(const PartialStateIterator&, const PartialStateEndIterator&);
    };

    class PartialStateEndIterator {
        friend AbstractStateMapper;

        friend bool
        operator==(const PartialStateIterator&, const PartialStateEndIterator&);

        friend bool
        operator!=(const PartialStateIterator&, const PartialStateEndIterator&);
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

    CartesianSubsetIterator
    cartesian_subsets_begin(std::vector<int> values, std::vector<int> indices)
        const;

    CartesianSubsetEndIterator cartesian_subsets_end() const;

    PartialStateIterator
    partial_states_begin(AbstractState offset, std::vector<int> indices) const;

    PartialStateEndIterator partial_states_end() const;

    int get_multiplier(int var) const;
    int get_multiplier_raw(int idx) const;

    int get_domain_size(int var) const;
    int get_domain_size_raw(int idx) const;

    int get_index(int var) const;

private:
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