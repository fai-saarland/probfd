#ifndef MDPS_HEURISTICS_PDBS_ABSTRACT_STATE_MAPPER_H
#define MDPS_HEURISTICS_PDBS_ABSTRACT_STATE_MAPPER_H

#include "../../../utils/iterators.h"
#include "../../../utils/range_proxy.h"
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
    struct VariableInfo {
        int var;
        int domain;
        int multiplier;
        int partial_multiplier;
    };

    class CartesianSubsetIterator {
        using difference_type = void;
        using value_type = std::vector<std::pair<int, int>>;
        using pointer = std::vector<std::pair<int, int>>*;
        using reference = std::vector<std::pair<int, int>>&;
        using iterator_category = std::forward_iterator_tag;

        std::vector<std::pair<int, int>> partial_state_;
        const std::vector<VariableInfo>& var_infos_;

        bool done;

    public:
        CartesianSubsetIterator(
            std::vector<std::pair<int, int>> partial_state,
            const std::vector<VariableInfo>& var_infos);

        CartesianSubsetIterator& operator++();
        CartesianSubsetIterator& operator--();

        reference operator*();
        pointer operator->();

        friend bool operator==(
            const CartesianSubsetIterator& a,
            const utils::default_sentinel_t&);

        friend bool operator!=(
            const CartesianSubsetIterator& a,
            const utils::default_sentinel_t&);
    };

    class PartialStateIterator {
        using difference_type = void;
        using value_type = const AbstractState;
        using pointer = const AbstractState*;
        using reference = const AbstractState&;
        using iterator_category = std::forward_iterator_tag;

        std::vector<int> values_;
        std::vector<int> domains_;
        std::vector<int> multipliers_;
        AbstractState state_;

        bool done;

    public:
        PartialStateIterator(
            AbstractState state,
            const std::vector<int>& indices,
            const std::vector<VariableInfo>& var_infos);

        PartialStateIterator& operator++();

        reference operator*();
        pointer operator->();

        friend bool operator==(
            const PartialStateIterator&,
            const utils::default_sentinel_t&);

        friend bool operator!=(
            const PartialStateIterator&,
            const utils::default_sentinel_t&);
    };

public:
    AbstractStateMapper(Pattern pattern, const std::vector<int>& domains);
    ~AbstractStateMapper() = default;

    unsigned int size() const;

    unsigned int num_vars() const;

    const Pattern& get_pattern() const;

    const std::vector<VariableInfo>& get_variable_infos() const
    {
        return var_infos_;
    }

    auto domains_begin() const
    {
        return utils::make_transform_iterator(
            var_infos_.begin(),
            &VariableInfo::domain);
    }

    auto domains_end() const
    {
        return utils::make_transform_iterator(
            var_infos_.end(),
            &VariableInfo::domain);
    }

    auto domains() const
    {
        return utils::make_range(domains_begin(), domains_end());
    }

    AbstractState operator()(const GlobalState& state) const;
    AbstractState operator()(const std::vector<int>& state) const;
    AbstractState from_values(const std::vector<int>& values) const;
    AbstractState from_values_partial(
        const std::vector<int>& indices,
        const std::vector<int>& values) const;
    AbstractState from_values_partial(
        const std::vector<std::pair<int, int>>& sparse_values) const;
    AbstractState from_values_partial(
        const std::vector<int>& indices,
        const std::vector<std::pair<int, int>>& sparse_values) const;
    AbstractState from_value_partial(int idx, int val) const;

    int get_unique_partial_state_id(
        const std::vector<int>& indices,
        const std::vector<int>& values) const;

    int get_unique_partial_state_id(
        const std::vector<std::pair<int, int>>& pstate) const;

    std::vector<int> to_values(AbstractState abstract_state) const;

    AbstractState
    convert(AbstractState abstract_state, const Pattern& values) const;

    void
    to_values(AbstractState abstract_state, std::vector<int>& values) const;

    CartesianSubsetIterator cartesian_subsets_begin(
        std::vector<std::pair<int, int>> partial_state) const;

    utils::default_sentinel_t cartesian_subsets_end() const;

    utils::RangeProxy<CartesianSubsetIterator, utils::default_sentinel_t>
    cartesian_subsets(std::vector<std::pair<int, int>> partial_state) const;

    PartialStateIterator
    partial_states_begin(AbstractState offset, std::vector<int> indices) const;

    utils::default_sentinel_t partial_states_end() const;

    utils::RangeProxy<PartialStateIterator, utils::default_sentinel_t>
    partial_states(AbstractState offset, std::vector<int> indices) const;

    int get_multiplier(int var) const;
    int get_multiplier_raw(int idx) const;

    int get_domain_size(int var) const;
    int get_domain_size_raw(int idx) const;

    int get_index(int var) const;

private:
    Pattern pattern_;
    std::vector<VariableInfo> var_infos_;
    int num_states_;
    int num_partial_states_;
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
#endif // __ABSTRACT_STATE_MAPPER_H__