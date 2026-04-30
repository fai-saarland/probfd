#include "probfd/merge_and_shrink/merge_strategy_sccs_selector.h"

#include "probfd/merge_and_shrink/factored_transition_system.h"
#include "probfd/merge_and_shrink/merge_selector.h"
#include "probfd/merge_and_shrink/transition_system.h"

#include <cassert>
#include <ranges>

using namespace std;

namespace probfd::merge_and_shrink {

MergeStrategySCCsSelector::MergeStrategySCCsSelector(
    const FactoredTransitionSystem& fts,
    SharedProbabilisticTask task,
    const shared_ptr<MergeSelector>& merge_selector,
    vector<vector<int>>&& non_singleton_cg_sccs)
    : MergeStrategy(fts)
    , task(std::move(task))
    , merge_selector(merge_selector)
    , non_singleton_cg_sccs(std::move(non_singleton_cg_sccs))
{
    std::ranges::reverse(non_singleton_cg_sccs);
}

MergeStrategySCCsSelector::~MergeStrategySCCsSelector() = default;

pair<int, int> MergeStrategySCCsSelector::get_next()
{
    if (current_ts_indices.empty()) {
        /*
          We are currently not dealing with merging all factors of an SCC, so
          we need to either get the next one or allow merging any existing
          factors of the FTS if there is no SCC left.
        */
        if (non_singleton_cg_sccs.empty()) {
            // We are done dealing with all SCCs, allow merging any factors.
            current_ts_indices.reserve(fts.get_num_active_entries());
            for (int ts_index : fts) { current_ts_indices.push_back(ts_index); }
        } else {
            /*
              There is another SCC we have to deal with. Store its factors so
              that we merge them over the next iterations.
            */
            vector<int>& current_scc = non_singleton_cg_sccs.back();
            assert(current_scc.size() > 1);
            current_ts_indices = std::move(current_scc);
            non_singleton_cg_sccs.pop_back();
        }
    } else {
        // Add the most recent product to the current index set.
        current_ts_indices.push_back(fts.get_size() - 1);
    }

    // Select the next merge from the current index set, using the selector.
    pair<int, int> next_pair =
        merge_selector->select_merge(fts, current_ts_indices);

    // Remove the two merged indices from the current index set.
    erase_if(current_ts_indices, [=](const int idx) {
        return idx == next_pair.first || idx == next_pair.second;
    });

    return next_pair;
}

bool MergeStrategySCCsSelector::requires_liveness() const
{
    return merge_selector->requires_liveness();
}

bool MergeStrategySCCsSelector::requires_goal_distances() const
{
    return merge_selector->requires_goal_distances();
}

} // namespace probfd::merge_and_shrink
