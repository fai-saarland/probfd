#ifndef MDPS_HEURISTICS_PDBS_ABSTRACT_STATE_MAPPER_H
#define MDPS_HEURISTICS_PDBS_ABSTRACT_STATE_MAPPER_H

#include "probfd/heuristics/pdbs/state_rank.h"
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

/**
 * @brief Implements a state ranking function for states.
 *
 * A state ranking function is a bijective mapping from states to so-called
 * state ranks, which are indices in \{ 0, \dots, N-1 \}, where N is the total
 * number of states. This class implements the traditional PDB (un-)ranking
 * functions as stated in \cite sievers:etal:socs-12 .
 *
 * Additionally, this class implements a ranking function for partial states of
 * a PDB by extending the domain of every variable by a special "missing" value
 * that is assumed in case of missing variables in a partial state.
 */
class StateRankingFunction {
    struct VariableInfo {
        int domain;
        long long int multiplier;
        long long int partial_multiplier;
    };

    Pattern pattern_;
    std::vector<VariableInfo> var_infos_;
    long long int num_states_;

public:
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

    class StateRankIterator {
        using difference_type = void;
        using value_type = const StateRank;
        using pointer = const StateRank*;
        using reference = const StateRank&;
        using iterator_category = std::forward_iterator_tag;

        std::vector<int> values_;
        std::vector<int> domains_;
        std::vector<long long int> multipliers_;
        StateRank state_;

        bool done;

    public:
        StateRankIterator(
            StateRank state,
            const std::vector<int>& indices,
            const std::vector<VariableInfo>& var_infos);

        StateRankIterator& operator++();

        reference operator*();
        pointer operator->();

        friend bool
        operator==(const StateRankIterator&, const utils::default_sentinel_t&);

        friend bool
        operator!=(const StateRankIterator&, const utils::default_sentinel_t&);
    };

public:
    /**
     * @brief Constructs the ranking function for a pattern database as
     * specified by a given pattern and the corresponding variable domains.
     */
    StateRankingFunction(Pattern pattern, const std::vector<int>& domains);

    unsigned int num_states() const;

    unsigned int num_vars() const;

    const Pattern& get_pattern() const;

    /**
     * @brief Ranks a given state.
     */
    template <typename StateLike>
    StateRank rank(const StateLike& state) const
    {
        StateRank res(0);
        for (size_t i = 0; i != pattern_.size(); ++i) {
            res.id += var_infos_[i].multiplier * state[pattern_[i]];
        }
        return res;
    }

    /**
     * @brief Unranks a given state rank. The state is returned as a vector.
     */
    std::vector<int> unrank(StateRank abstract_state) const;

    StateRank from_values_partial(
        const std::vector<std::pair<int, int>>& sparse_values) const;
    StateRank from_values_partial(
        const std::vector<int>& indices,
        const std::vector<std::pair<int, int>>& sparse_values) const;
    StateRank from_fact(int idx, int val) const;

    /**
     * @brief Ranks a given partial state.
     */
    long long int get_unique_partial_state_id(
        const std::vector<std::pair<int, int>>& pstate) const;

    StateRank convert(StateRank abstract_state, const Pattern& values) const;

    PartialAssignmentIterator partial_assignments_begin(
        std::vector<std::pair<int, int>> partial_state) const;

    utils::default_sentinel_t partial_assignments_end() const;

    utils::RangeProxy<PartialAssignmentIterator, utils::default_sentinel_t>
    partial_assignments(std::vector<std::pair<int, int>> partial_state) const;

    StateRankIterator
    state_ranks_begin(StateRank offset, std::vector<int> indices) const;

    utils::default_sentinel_t state_ranks_end() const;

    utils::RangeProxy<StateRankIterator, utils::default_sentinel_t>
    state_ranks(StateRank offset, std::vector<int> indices) const;

    long long int get_multiplier(int var) const;

    int get_domain_size(int var) const;

    int get_index(int var) const;
};

class StateRankToString {
    std::shared_ptr<StateRankingFunction> state_mapper_;

public:
    explicit StateRankToString(
        std::shared_ptr<StateRankingFunction> state_mapper);
    std::string operator()(const StateID& id, StateRank state) const;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
#endif // __ABSTRACT_STATE_MAPPER_H__