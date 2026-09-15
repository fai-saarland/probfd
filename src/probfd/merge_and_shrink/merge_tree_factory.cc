#include "probfd/merge_and_shrink/merge_tree_factory.h"

#include "downward/merge_and_shrink/merge_tree.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng_options.h"
#include "downward/utils/system.h"

#include <iostream>

using namespace std;
using namespace downward;

namespace probfd::merge_and_shrink {

MergeTreeFactory::MergeTreeFactory(
    std::shared_ptr<downward::merge_and_shrink::MergeUpdateStrategy>
        merge_update_strategy)
    : merge_update_strategy(std::move(merge_update_strategy))
{
}

void MergeTreeFactory::dump_options(utils::LogProxy& log) const
{
    log.println("Merge tree options:");
    log.println("Type: {}", name());
    log.print("Update option: ");
    merge_update_strategy->dump_options(log);
    log.println();
    dump_tree_specific_options(log);
}

unique_ptr<downward::merge_and_shrink::MergeTree>
MergeTreeFactory::compute_merge_tree(
    const SharedProbabilisticTask&,
    const FactoredTransitionSystem&,
    const vector<int>&)
{
    throw utils::CriticalError(
        "This merge tree does not support being computed on a subset "
        "of indices for a given factored transition system!");
}

} // namespace probfd::merge_and_shrink