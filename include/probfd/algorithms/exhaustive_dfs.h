#ifndef PROBFD_ALGORITHMS_EXHAUSTIVE_DFS_H
#define PROBFD_ALGORITHMS_EXHAUSTIVE_DFS_H

#include "probfd/algorithms/state_properties.h"

#include "probfd/algorithms/types.h"

#include "probfd/storage/per_state_storage.h"

#include "iterative_mdp_algorithm.h"
#include "probfd/distribution.h"
#include "probfd/progress_report.h"

#include <deque>
#include <limits>

// Forward Declarations
namespace probfd::algorithms {
template <typename, typename>
class TransitionSorter;
}

/// namespace for anytime TVI
namespace probfd::algorithms::exhaustive_dfs {

struct Statistics {
    unsigned long long expanded = 0;
    unsigned long long evaluations = 0;
    unsigned long long evaluated = 0;
    unsigned long long goal_states = 0;
    unsigned long long transitions = 0;
    unsigned long long dead_ends = 0;
    unsigned long long terminal = 0;
    unsigned long long self_loop = 0;

    unsigned long long value_updates = 0;
    unsigned long long backtracks = 0;
    unsigned long long sccs = 0;
    unsigned long long dead_end_sccs = 0;
    unsigned long long pruned_dead_end_sccs = 0;
    unsigned long long summed_dead_end_scc_sizes = 0;

    void print(std::ostream& out) const;
};

struct ExpansionInformation {
    explicit ExpansionInformation(unsigned stack_index)
        : stack_index(stack_index)
    {
    }

    std::vector<Distribution<StateID>> successors;
    Distribution<StateID>::const_iterator succ;
    unsigned stack_index;

    bool all_successors_are_dead = true;
    bool all_successors_marked_dead = true;

    void update_successors_dead(bool successors_dead)
    {
        all_successors_are_dead = all_successors_are_dead && successors_dead;
    }
};

struct SCCTransition {
    Distribution<StateID> successors;
    value_t base = 0_vt;
    value_t self_loop = 0_vt;
};

struct StackInformation {
    explicit StackInformation(StateID state_ref)
        : state_ref(state_ref)
    {
    }

    StateID state_ref;
    std::vector<SCCTransition> successors;
    int i = -1;
};

template <bool UseInterval>
struct SearchNodeInformation {
    static constexpr uint8_t NEW = 0;
    static constexpr uint8_t CLOSED = 1;
    static constexpr uint8_t OPEN = 2;
    static constexpr uint8_t ONSTACK = 4;
    static constexpr uint8_t DEAD = 3;
    static constexpr uint8_t MARKED = 7;

    // TODO store lowlink in hash map -> only required for states still on
    // stack
    unsigned lowlink = std::numeric_limits<unsigned int>::max();
    uint8_t status = NEW;
    AlgorithmValue<UseInterval> value;
    value_t term_cost;

    [[nodiscard]]
    bool is_new() const
    {
        return status == NEW;
    }
    [[nodiscard]]
    bool is_open() const
    {
        return status == OPEN;
    }
    [[nodiscard]]
    bool is_onstack() const
    {
        return status == ONSTACK;
    }
    [[nodiscard]]
    bool is_closed() const
    {
        return status & CLOSED;
    }
    [[nodiscard]]
    bool is_dead_end() const
    {
        return status == DEAD || status == MARKED;
    }
    [[nodiscard]]
    bool is_marked_dead_end() const
    {
        return status == MARKED;
    }

    void open()
    {
        assert(is_new());
        status = OPEN;
    }

    void set_onstack(const unsigned idx)
    {
        assert(is_open());
        status = ONSTACK;
        lowlink = idx;
    }

    void close() { status = CLOSED; }

    void set_dead_end()
    {
        assert(!is_marked_dead_end());
        status = DEAD;
    }

    void mark_dead_end() { status = MARKED; }

    [[nodiscard]]
    value_t get_value() const
    {
        if constexpr (UseInterval) {
            return value.lower;
        } else {
            return value;
        }
    }

    [[nodiscard]]
    Interval get_bounds() const
    {
        if constexpr (!UseInterval) {
            return Interval(value, INFINITE_VALUE);
        } else {
            return value;
        }
    }
};

template <bool UseInterval>
struct SearchNodeInfos : public StateProperties {
    storage::PerStateStorage<SearchNodeInformation<UseInterval>> infos;

    SearchNodeInformation<UseInterval>& operator[](StateID state_id)
    {
        return infos[state_id];
    }

    const SearchNodeInformation<UseInterval>& operator[](StateID state_id) const
    {
        return infos[state_id];
    }

    value_t lookup_value(StateID state_id) override
    {
        return infos[state_id].get_value();
    }

    Interval lookup_bounds(StateID state_id) override
    {
        return infos[state_id].get_bounds();
    }
};

/**
 * @brief Implementation of an anytime topological value iteration
 * variant.
 *
 * \todo Add some implementation notes.
 *
 * @tparam State - The state type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam UseInterval - Whether value intervals are used.
 *
 */
template <typename State, typename Action, bool UseInterval>
class ExhaustiveDepthFirstSearch : public IterativeMDPAlgorithm<State, Action> {
    using Base = typename ExhaustiveDepthFirstSearch::IterativeMDPAlgorithm;

    using MDPType = typename Base::MDPType;
    using HeuristicType = typename Base::HeuristicType;
    using PolicyType = Base::PolicyType;

    using TransitionSorterType = TransitionSorter<State, Action>;

    using AlgorithmValueType = AlgorithmValue<UseInterval>;
    using SearchNodeInfo = SearchNodeInformation<UseInterval>;

    // Algorithm parameters
    const std::shared_ptr<TransitionSorterType> transition_sort_;

    const Interval cost_bound_;
    const AlgorithmValueType trivial_bound_;

    const bool value_propagation_;
    const bool only_propagate_when_changed_;

    // Algorithm state
    SearchNodeInfos<UseInterval> search_space_;

    std::deque<ExpansionInformation> expansion_infos_;
    std::deque<StackInformation> stack_infos_;
    std::vector<StateID> neighbors_;

    bool last_all_dead_ = true;
    bool last_all_marked_dead_ = true;

    Statistics statistics_;

public:
    explicit ExhaustiveDepthFirstSearch(
        value_t epsilon,
        std::shared_ptr<TransitionSorterType> transition_sorting,
        Interval cost_bound,
        bool path_updates,
        bool only_propagate_when_changed);

    Interval solve(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> state,
        ProgressReport progress,
        double max_time) override;

    std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> state,
        ProgressReport progress,
        double max_time) override;

    void print_statistics(std::ostream& out) const override;

private:
    void register_value_reports(
        const SearchNodeInfo& info,
        ProgressReport& progress);

    bool initialize_search_node(
        MDPType& mdp,
        HeuristicType& heuristic,
        StateID state_id,
        SearchNodeInfo& info);

    bool initialize_search_node(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> state,
        SearchNodeInfo& info);

    bool push_state(
        MDPType& mdp,
        HeuristicType& heuristic,
        StateID state_id,
        SearchNodeInfo& info);

    void run_exploration(
        MDPType& mdp,
        HeuristicType& heuristic,
        ProgressReport& progress);

    void propagate_value_along_trace(
        bool was_poped,
        value_t val,
        ProgressReport& progress);

    bool check_early_convergence(const SearchNodeInfo& node) const;
};

} // namespace probfd::algorithms::exhaustive_dfs

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_EXHAUSTIVE_DFS_H
#include "probfd/algorithms/exhaustive_dfs_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_EXHAUSTIVE_DFS_H

#endif // PROBFD_ALGORITHMS_EXHAUSTIVE_DFS_H
