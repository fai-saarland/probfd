#ifndef MDPS_HEURISTICS_PDBS_ABSTRACT_STATE_MAPPER_H
#define MDPS_HEURISTICS_PDBS_ABSTRACT_STATE_MAPPER_H

#include "probfd/heuristics/pdbs/abstract_state.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/types.h"

#include "utils/iterators.h"
#include "utils/range_proxy.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

class GlobalState;

namespace probfd {
namespace heuristics {
namespace pdbs {

class AbstractStateMapper {
public:
    struct VariableInfo {
        int domain;
        long long int multiplier;
        long long int partial_multiplier;
    };

    class PartialAssignmentIterator {
        using difference_type = void;
        using value_type = std::vector<std::pair<int, int>>;
        using pointer = std::vector<std::pair<int, int>>*;
        using reference = std::vector<std::pair<int, int>>&;
        using iterator_category = std::forward_iterator_tag;

        std::vector<std::pair<int, int>> partial_state_;
        const std::vector<VariableInfo>& var_infos_;

        bool done;

    public:
        PartialAssignmentIterator(
            std::vector<std::pair<int, int>> partial_state,
            const std::vector<VariableInfo>& var_infos);

        PartialAssignmentIterator& operator++();
        PartialAssignmentIterator& operator--();

        reference operator*();
        pointer operator->();

        friend bool operator==(
            const PartialAssignmentIterator& a,
            const utils::default_sentinel_t&);

        friend bool operator!=(
            const PartialAssignmentIterator& a,
            const utils::default_sentinel_t&);
    };

    class AbstractStateIterator {
        using difference_type = void;
        using value_type = const AbstractState;
        using pointer = const AbstractState*;
        using reference = const AbstractState&;
        using iterator_category = std::forward_iterator_tag;

        std::vector<int> values_;
        std::vector<int> domains_;
        std::vector<long long int> multipliers_;
        AbstractState state_;

        bool done;

    public:
        AbstractStateIterator(
            AbstractState state,
            const std::vector<int>& indices,
            const std::vector<VariableInfo>& var_infos);

        AbstractStateIterator& operator++();

        reference operator*();
        pointer operator->();

        friend bool operator==(
            const AbstractStateIterator&,
            const utils::default_sentinel_t&);

        friend bool operator!=(
            const AbstractStateIterator&,
            const utils::default_sentinel_t&);
    };

public:
    AbstractStateMapper(Pattern pattern, const std::vector<int>& domains);
    ~AbstractStateMapper() = default;

    unsigned int num_states() const;

    unsigned int num_vars() const;

    const Pattern& get_pattern() const;

    template <typename State>
    AbstractState rank(const State& state) const
    {
        AbstractState res(0);
        for (size_t i = 0; i != pattern_.size(); ++i) {
            res.id += var_infos_[i].multiplier * state[pattern_[i]];
        }
        return res;
    }

    std::vector<int> unrank(AbstractState abstract_state) const;

    AbstractState from_values_partial(
        const std::vector<std::pair<int, int>>& sparse_values) const;
    AbstractState from_values_partial(
        const std::vector<int>& indices,
        const std::vector<std::pair<int, int>>& sparse_values) const;
    AbstractState from_fact(int idx, int val) const;

    long long int get_unique_partial_state_id(
        const std::vector<std::pair<int, int>>& pstate) const;

    AbstractState
    convert(AbstractState abstract_state, const Pattern& values) const;

    PartialAssignmentIterator partial_assignments_begin(
        std::vector<std::pair<int, int>> partial_state) const;

    utils::default_sentinel_t partial_assignments_end() const;

    utils::RangeProxy<PartialAssignmentIterator, utils::default_sentinel_t>
    partial_assignments(std::vector<std::pair<int, int>> partial_state) const;

    AbstractStateIterator
    abstract_states_begin(AbstractState offset, std::vector<int> indices) const;

    utils::default_sentinel_t abstract_states_end() const;

    utils::RangeProxy<AbstractStateIterator, utils::default_sentinel_t>
    abstract_states(AbstractState offset, std::vector<int> indices) const;

    long long int get_multiplier(int var) const;

    int get_domain_size(int var) const;

    int get_index(int var) const;

private:
    Pattern pattern_;
    std::vector<VariableInfo> var_infos_;
    long long int num_states_;
};

class AbstractStateToString {
public:
    explicit AbstractStateToString(
        std::shared_ptr<AbstractStateMapper> state_mapper);
    std::string operator()(const StateID& id, AbstractState state) const;

private:
    std::shared_ptr<AbstractStateMapper> state_mapper_;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
#endif // __ABSTRACT_STATE_MAPPER_H__