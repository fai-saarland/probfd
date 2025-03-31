#include "probfd/merge_and_shrink/merge_selector.h"
#include "probfd/merge_and_shrink/factored_transition_system.h"

#include "downward/utils/logging.h"

#include <cassert>

using namespace std;

namespace probfd::merge_and_shrink {

vector<pair<int, int>> MergeSelector::compute_merge_candidates(
    const FactoredTransitionSystem& fts,
    const vector<int>& indices_subset)
{
    vector<pair<int, int>> merge_candidates;

    if (indices_subset.empty()) {
        for (int ts_index1 = 0; ts_index1 < fts.get_size(); ++ts_index1) {
            if (!fts.is_active(ts_index1)) continue;
            for (int ts_index2 = ts_index1 + 1; ts_index2 < fts.get_size();
                 ++ts_index2) {
                if (fts.is_active(ts_index2)) {
                    merge_candidates.emplace_back(ts_index1, ts_index2);
                }
            }
        }
    } else {
        assert(indices_subset.size() > 1);
        for (size_t i = 0; i < indices_subset.size(); ++i) {
            int ts_index1 = indices_subset[i];
            assert(fts.is_active(ts_index1));
            for (size_t j = i + 1; j < indices_subset.size(); ++j) {
                int ts_index2 = indices_subset[j];
                assert(fts.is_active(ts_index2));
                merge_candidates.emplace_back(ts_index1, ts_index2);
            }
        }
    }

    return merge_candidates;
}

void MergeSelector::dump_options(utils::LogProxy& log) const
{
    if (log.is_at_least_normal()) {
        log << "Merge selector options:" << endl;
        log << "Name: " << name() << endl;
        dump_selector_specific_options(log);
    }
}

} // namespace probfd::merge_and_shrink