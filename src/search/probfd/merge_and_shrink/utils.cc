#include "probfd/merge_and_shrink/utils.h"

#include "probfd/merge_and_shrink/distances.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/shrink_strategy.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include "downward/utils/logging.h"
#include "downward/utils/math.h"
#include "downward/utils/timer.h"

#include <cassert>
#include <cmath>

using namespace std;

namespace probfd::merge_and_shrink {

pair<int, int> compute_shrink_sizes(
    int size1,
    int size2,
    int max_states_before_merge,
    int max_states_after_merge)
{
    // Bound both sizes by max allowed size before merge.
    int new_size1 = min(size1, max_states_before_merge);
    int new_size2 = min(size2, max_states_before_merge);

    if (!utils::is_product_within_limit(
            new_size1,
            new_size2,
            max_states_after_merge)) {
        int balanced_size = int(sqrt(max_states_after_merge));

        if (new_size1 <= balanced_size) {
            // Size of the first transition system is small enough. Use whatever
            // is left for the second transition system.
            new_size2 = max_states_after_merge / new_size1;
        } else if (new_size2 <= balanced_size) {
            // Inverted case as before.
            new_size1 = max_states_after_merge / new_size2;
        } else {
            // Both transition systems are too big. We set both target sizes
            // to balanced_size. An alternative would be to set one to
            // N1 = balanced_size and the other to N2 = max_states_after_merge /
            // balanced_size, to get closer to the allowed maximum.
            // However, this would make little difference (N2 would
            // always be N1, N1 + 1 or N1 + 2), and our solution has the
            // advantage of treating the transition systems symmetrically.
            new_size1 = balanced_size;
            new_size2 = balanced_size;
        }
    }
    assert(new_size1 <= size1 && new_size2 <= size2);
    assert(new_size1 <= max_states_before_merge);
    assert(new_size2 <= max_states_before_merge);
    assert(new_size1 * new_size2 <= max_states_after_merge);
    return make_pair(new_size1, new_size2);
}

/*
  This method checks if the transition system of the factor at index violates
  the size limit given via new_size (e.g. as computed by compute_shrink_sizes)
  or the threshold shrink_threshold_before_merge that triggers shrinking even
  if the size limit is not violated. If so, trigger the shrinking process.
  Return true iff the factor was actually shrunk.
*/
static bool shrink_factor(
    FactoredTransitionSystem& fts,
    int index,
    int new_size,
    int shrink_threshold_before_merge,
    const ShrinkStrategy& shrink_strategy,
    utils::Timer& shrink_strategy_timer,
    utils::LogProxy& log)
{
    /*
      TODO: think about factoring out common logic of this function and the
      function copy_and_shrink_ts in merge_scoring_function_miasm_utils.cc.
    */
    const TransitionSystem& ts = fts.get_transition_system(index);
    int num_states = ts.get_size();
    if (num_states > min(new_size, shrink_threshold_before_merge)) {
        if (log.is_at_least_verbose()) {
            log << ts.tag() << "current size: " << num_states;
            if (new_size < num_states)
                log << " (new size limit: " << new_size;
            else
                log << " (shrink threshold: " << shrink_threshold_before_merge;
            log << ")" << endl;
        }

        const Distances& distances = fts.get_distances(index);
        shrink_strategy_timer.resume();
        StateEquivalenceRelation equivalence_relation =
            shrink_strategy
                .compute_equivalence_relation(ts, distances, new_size, log);
        shrink_strategy_timer.stop();
        // TODO: We currently violate this; see issue250
        // assert(equivalence_relation.size() <= target_size);
        return fts.apply_abstraction(index, equivalence_relation, log);
    }
    return false;
}

bool shrink_before_merge_step(
    FactoredTransitionSystem& fts,
    int index1,
    int index2,
    int max_states,
    int max_states_before_merge,
    int shrink_threshold_before_merge,
    const ShrinkStrategy& shrink_strategy,
    utils::Timer& shrink_strategy_timer,
    utils::LogProxy& log)
{
    /*
      Compute the size limit for both transition systems as imposed by
      max_states and max_states_before_merge.
    */
    pair<int, int> new_sizes = compute_shrink_sizes(
        fts.get_transition_system(index1).get_size(),
        fts.get_transition_system(index2).get_size(),
        max_states_before_merge,
        max_states);

    /*
      For both transition systems, possibly compute and apply an
      abstraction.
      TODO: we could better use the given limit by increasing the size limit
      for the second shrinking if the first shrinking was larger than
      required.
    */
    bool shrunk1 = shrink_factor(
        fts,
        index1,
        new_sizes.first,
        shrink_threshold_before_merge,
        shrink_strategy,
        shrink_strategy_timer,
        log);
    if (shrunk1) {
        fts.statistics(index1, log);
    }
    bool shrunk2 = shrink_factor(
        fts,
        index2,
        new_sizes.second,
        shrink_threshold_before_merge,
        shrink_strategy,
        shrink_strategy_timer,
        log);
    if (shrunk2) {
        fts.statistics(index2, log);
    }
    return shrunk1 || shrunk2;
}

vector<int> compute_abstraction_mapping(
    int num_states,
    const StateEquivalenceRelation& equivalence_relation)
{
    vector<int> abstraction_mapping(num_states, PRUNED_STATE);
    for (size_t class_no = 0; class_no < equivalence_relation.size();
         ++class_no) {
        const StateEquivalenceClass& state_equivalence_class =
            equivalence_relation[class_no];
        for (int state : state_equivalence_class) {
            assert(abstraction_mapping[state] == PRUNED_STATE);
            abstraction_mapping[state] = class_no;
        }
    }
    return abstraction_mapping;
}

bool is_goal_relevant(const TransitionSystem& ts)
{
    int num_states = ts.get_size();
    for (int state = 0; state < num_states; ++state) {
        if (!ts.is_goal_state(state)) {
            return true;
        }
    }
    return false;
}

} // namespace probfd::merge_and_shrink