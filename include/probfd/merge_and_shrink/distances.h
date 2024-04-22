#ifndef PROBFD_MERGE_AND_SHRINK_DISTANCES_H
#define PROBFD_MERGE_AND_SHRINK_DISTANCES_H

#include "probfd/merge_and_shrink/types.h"

#include "probfd/heuristics/constant_heuristic.h"

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

namespace utils {
class LogProxy;
}

namespace probfd::merge_and_shrink {
class TransitionSystem;
}

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

    std::vector<value_t> extract_goal_distances()
    {
        return std::move(goal_distances);
    }

    void compute_distances(
        const TransitionSystem& transition_system,
        bool compute_liveness,
        utils::LogProxy& log,
        const Heuristic<int>& heuristic =
            heuristics::ConstantEvaluator<int>(0_vt));

    /*
      Update distances according to the given abstraction. If the abstraction
      is not f-preserving, distances are directly recomputed.

      It is OK for the abstraction to drop states, but then all
      dropped states must be unreachable or irrelevant. (Otherwise,
      the method might fail to detect that the distance information is
      out of date.)
    */
    void apply_abstraction(
        const TransitionSystem& transition_system,
        const StateEquivalenceRelation& state_equivalence_relation,
        bool compute_liveness,
        utils::LogProxy& log);

    void dump(utils::LogProxy& log) const;
    void
    statistics(const TransitionSystem& transition_system, utils::LogProxy& log)
        const;
};

void compute_liveness(
    const TransitionSystem& transition_system,
    std::span<const value_t> goal_distances,
    std::vector<bool>& liveness,
    std::vector<int> queue = std::vector<int>());

void compute_goal_distances(
    const TransitionSystem& transition_system,
    std::span<value_t> distances,
    const Heuristic<int>& heuristic = heuristics::ConstantEvaluator<int>(0_vt));

} // namespace probfd::merge_and_shrink

#endif
