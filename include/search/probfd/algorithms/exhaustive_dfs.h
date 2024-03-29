#ifndef PROBFD_ALGORITHMS_EXHAUSTIVE_DFS_H
#define PROBFD_ALGORITHMS_EXHAUSTIVE_DFS_H

#include "probfd/algorithms/state_properties.h"

#include "probfd/algorithms/types.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/mdp_algorithm.h"
#include "probfd/progress_report.h"

#include <deque>

namespace probfd {
namespace algorithms {

template <typename, typename>
class TransitionSorter;

/// namespace for anytime TVI
namespace exhaustive_dfs {

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
class ExhaustiveDepthFirstSearch : public MDPAlgorithm<State, Action> {
    using Base = typename ExhaustiveDepthFirstSearch::MDPAlgorithm;

    using MDP = typename Base::MDP;
    using Evaluator = typename Base::Evaluator;

    using TransitionSorter = TransitionSorter<State, Action>;

    using AlgorithmValueType = AlgorithmValueType<UseInterval>;

    struct SearchNodeInformation {
        static constexpr uint8_t NEW = 0;
        static constexpr uint8_t CLOSED = 1;
        static constexpr uint8_t OPEN = 2;
        static constexpr uint8_t ONSTACK = 4;
        static constexpr uint8_t DEAD = 3;
        static constexpr uint8_t MARKED = 7;

        // TODO store lowlink in hash map -> only required for states still on
        // stack
        unsigned lowlink = -1;
        uint8_t status = NEW;
        AlgorithmValueType value;
        value_t term_cost;

        bool is_new() const { return status == NEW; }
        bool is_open() const { return status == OPEN; }
        bool is_onstack() const { return status == ONSTACK; }
        bool is_closed() const { return status & CLOSED; }
        bool is_dead_end() const { return status == DEAD || status == MARKED; }
        bool is_marked_dead_end() const { return status == MARKED; }

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

        value_t get_value() const
        {
            if constexpr (UseInterval) {
                return value.lower;
            } else {
                return value;
            }
        }

        Interval get_bounds() const
        {
            if constexpr (!UseInterval) {
                return Interval(value, INFINITE_VALUE);
            } else {
                return value;
            }
        }
    };

    struct ExpansionInformation {
        explicit ExpansionInformation(
            unsigned stack_index,
            unsigned neighbors_size)
            : stack_index(stack_index)
            , neighbors_size(neighbors_size)
        {
        }

        std::vector<Distribution<StateID>> successors;
        Distribution<StateID>::const_iterator succ;
        unsigned stack_index;
        unsigned neighbors_size;

        bool all_successors_are_dead = true;
        bool all_successors_marked_dead = true;

        void update_successors_dead(bool successors_dead)
        {
            all_successors_are_dead =
                all_successors_are_dead && successors_dead;
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

    struct SearchNodeInfos : public StateProperties {
        storage::PerStateStorage<SearchNodeInformation> infos;

        SearchNodeInformation& operator[](StateID state_id)
        {
            return infos[state_id];
        }

        const SearchNodeInformation& operator[](StateID state_id) const
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

    std::shared_ptr<TransitionSorter> transition_sort_;

    ProgressReport* report_;
    const Interval cost_bound_;
    const AlgorithmValueType trivial_bound_;

    const bool value_propagation_;
    const bool only_propagate_when_changed_;
    const bool evaluator_recomputation_;
    const bool notify_initial_state_;

    Statistics statistics_;
    SearchNodeInfos search_space_;

    std::deque<ExpansionInformation> expansion_infos_;
    std::deque<StackInformation> stack_infos_;
    std::vector<StateID> neighbors_;

    bool last_all_dead_ = true;
    bool last_all_marked_dead_ = true;
    bool backtracked_from_dead_end_scc_ = false;

public:
    explicit ExhaustiveDepthFirstSearch(
        std::shared_ptr<TransitionSorter> transition_sorting,
        Interval cost_bound,
        bool reevaluate,
        bool notify_initial,
        bool path_updates,
        bool only_propagate_when_changed,
        ProgressReport* progress);

    Interval
    solve(MDP& mdp, Evaluator& heuristic, param_type<State> state, double)
        override;

    void print_statistics(std::ostream& out) const override;

private:
    void register_value_reports(const SearchNodeInformation& info);

    bool initialize_search_node(
        MDP& mdp,
        Evaluator& heuristic,
        StateID state_id,
        SearchNodeInformation& info);

    bool initialize_search_node(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        SearchNodeInformation& info);

    bool push_state(
        MDP& mdp,
        Evaluator& heuristic,
        StateID state_id,
        SearchNodeInformation& info);

    void run_exploration(MDP& mdp, Evaluator& heuristic);

    void propagate_value_along_trace(bool was_poped, value_t val);

    void mark_current_component_dead();

    bool check_early_convergence(const SearchNodeInformation& node) const;
};

} // namespace exhaustive_dfs
} // namespace algorithms
} // namespace probfd

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_EXHAUSTIVE_DFS_H
#include "probfd/algorithms/exhaustive_dfs_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_EXHAUSTIVE_DFS_H

#endif // __EXHAUSTIVE_DFS_H__
