#ifndef PROBFD_HEURISTICS_MERGE_AND_SHRINK_DISTANCES_H
#define PROBFD_HEURISTICS_MERGE_AND_SHRINK_DISTANCES_H

#include "probfd/merge_and_shrink/types.h"

#include "probfd/value_type.h"

#include <cassert>
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

class Distances {
    static constexpr value_t DISTANCE_UNKNOWN = -1;

    const TransitionSystem& transition_system;
    std::vector<value_t> init_distances;
    std::vector<value_t> goal_distances;
    bool init_distances_computed = false;
    bool goal_distances_computed = false;

    void compute_init_distances();
    void compute_goal_distances();

public:
    explicit Distances(const TransitionSystem& transition_system);

    bool are_init_distances_computed() const { return init_distances_computed; }
    bool are_goal_distances_computed() const { return goal_distances_computed; }

    value_t get_init_distance(int state) const
    {
        assert(are_init_distances_computed());
        return init_distances[state];
    }

    value_t get_goal_distance(int state) const
    {
        assert(are_goal_distances_computed());
        return goal_distances[state];
    }

    void compute_distances(
        bool compute_init_distances,
        bool compute_goal_distances,
        utils::LogProxy& log);

    /*
      Update distances according to the given abstraction. If the abstraction
      is not f-preserving, distances are directly recomputed.

      It is OK for the abstraction to drop states, but then all
      dropped states must be unreachable or irrelevant. (Otherwise,
      the method might fail to detect that the distance information is
      out of date.)
    */
    void apply_abstraction(
        const StateEquivalenceRelation& state_equivalence_relation,
        bool compute_init_distances,
        bool compute_goal_distances,
        utils::LogProxy& log);

    void dump(utils::LogProxy& log) const;
    void statistics(utils::LogProxy& log) const;
};

} // namespace probfd::merge_and_shrink

#endif
