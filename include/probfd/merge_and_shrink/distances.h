#ifndef PROBFD_MERGE_AND_SHRINK_DISTANCES_H
#define PROBFD_MERGE_AND_SHRINK_DISTANCES_H

#include "probfd/merge_and_shrink/types.h"

#include "probfd/value_type.h"

#include <cassert>
#include <span>
#include <vector>

/*
  TODO: Possible interface improvements for this class:
  - Check TODOs in implementation file.

  (Many of these would need performance tests, as distance computation
  can be one of the bottlenecks in our code.)
*/

namespace downward::utils {
class LogProxy;
}

namespace probfd {
template <typename State>
class Heuristic;
}

namespace probfd::merge_and_shrink {
class Labels;
class TransitionSystem;
} // namespace probfd::merge_and_shrink

namespace probfd::merge_and_shrink {

static constexpr value_t DISTANCE_UNKNOWN =
    std::numeric_limits<value_t>::quiet_NaN();

class Distances {
    std::vector<bool> liveness;
    std::vector<value_t> goal_distances;

    bool liveness_computed = false;
    bool goal_distances_computed = false;

public:
    bool is_liveness_computed() const { return liveness_computed; }

    bool are_goal_distances_computed() const { return goal_distances_computed; }

    bool is_alive(int state) const
    {
        assert(is_liveness_computed());
        return liveness[state];
    }

    value_t get_goal_distance(int state) const
    {
        assert(are_goal_distances_computed());
        return goal_distances[state];
    }

    value_t is_solvable(int state) const
    { return get_goal_distance(state) != INFINITE_VALUE; }

    std::vector<value_t> extract_goal_distances()
    { return std::move(goal_distances); }

    void compute_distances(
        const Labels& labels,
        const TransitionSystem& transition_system,
        bool compute_liveness,
        downward::utils::LogProxy& log);

    void compute_distances(
        const Labels& labels,
        const TransitionSystem& transition_system,
        bool compute_liveness,
        downward::utils::LogProxy& log,
        const Heuristic<int>& heuristic);

    /*
      Update distances according to the given abstraction. If the abstraction
      is not f-preserving, distances are directly recomputed.

      It is OK for the abstraction to drop states, but then all
      dropped states must be unreachable or irrelevant. (Otherwise,
      the method might fail to detect that the distance information is
      out of date.)
    */
    void apply_abstraction(
        const Labels& labels,
        const TransitionSystem& transition_system,
        const StateEquivalenceRelation& state_equivalence_relation,
        bool compute_liveness,
        downward::utils::LogProxy& log);

    void dump(downward::utils::LogProxy& log) const;

    void statistics(
        const TransitionSystem& transition_system,
        downward::utils::LogProxy& log) const;
};

/// Computes the forward-reachable states of a transition system for the given
/// initial state.
void compute_forward_reachability(
    const TransitionSystem& transition_system,
    std::predicate<int> auto is_state_pruned,
    std::vector<bool>& is_reachable,
    std::vector<int>& queue);

/// Computes the forward-reachable states of a transition system for the given
/// initial state.
void compute_forward_reachability(
    const TransitionSystem& transition_system,
    std::invocable<int> auto is_pruned,
    std::vector<bool>& reachability);

void compute_goal_distances(
    const Labels& labels,
    const TransitionSystem& transition_system,
    std::span<value_t> distances);

void compute_goal_distances(
    const Labels& labels,
    const TransitionSystem& transition_system,
    std::span<value_t> distances,
    const Heuristic<int>& heuristic);

} // namespace probfd::merge_and_shrink

#include "probfd/merge_and_shrink/transition_system.h"

#include <algorithm>
#include <ranges>

namespace probfd::merge_and_shrink {

void compute_forward_reachability(
    const TransitionSystem& transition_system,
    std::predicate<int> auto is_state_pruned,
    std::vector<bool>& is_reachable,
    std::vector<int>& queue)
{
    std::vector<std::vector<int>> forward_graph(transition_system.num_states());
    for (const auto& t = transition_system.get_transition_relation();
         const auto& local_label_info : t.label_infos()) {
        for (const auto& [src, targets] : local_label_info.get_transitions()) {
            // Skip pruned transitions
            if (std::ranges::any_of(targets, is_state_pruned)) continue;

            for (auto& edges = forward_graph[src]; int target : targets) {
                edges.emplace_back(target);
            }
        }
    }

    for (int init_state : queue) { is_reachable[init_state] = true; }

    do {
        const int state = queue.back();
        queue.pop_back();

        for (int successor : forward_graph[state]) {
            if (is_reachable[successor]) continue;
            is_reachable[successor] = true;
            queue.push_back(successor);
        }
    } while (!queue.empty());
}

void compute_forward_reachability(
    const TransitionSystem& transition_system,
    std::invocable<int> auto is_pruned,
    std::vector<bool>& reachability)
{
    std::vector<int> queue;
    queue.reserve(transition_system.num_states());

    for (int i = 0; i != transition_system.num_states(); ++i) {
        if (transition_system.is_init_state(i)) { queue.push_back(i); }
    }

    compute_forward_reachability(
        transition_system,
        is_pruned,
        reachability,
        queue);
}

} // namespace probfd::merge_and_shrink

#endif
