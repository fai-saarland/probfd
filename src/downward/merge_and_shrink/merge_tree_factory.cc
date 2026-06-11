#include "downward/merge_and_shrink/merge_tree_factory.h"

#include "downward/merge_and_shrink/merge_tree.h"

#include "downward/utils/exceptions.h"
#include "downward/utils/logging.h"

#include <iostream>

using namespace std;

namespace downward::merge_and_shrink {
MergeTreeFactory::MergeTreeFactory(
    std::shared_ptr<MergeUpdateStrategy> merge_update_strategy)
    : merge_update_strategy(std::move(merge_update_strategy))
{
}

void MergeTreeFactory::dump_options(utils::LogProxy& log) const
{
    log << "Merge tree options: " << endl;
    log << "Type: " << name() << endl;
    log << "Update option: ";
    merge_update_strategy->dump_options(log);
    log << endl;
    dump_tree_specific_options(log);
}

unique_ptr<MergeTree> MergeTreeFactory::compute_merge_tree(
    const AbstractTaskTuple&,
    const FactoredTransitionSystem&,
    const vector<int>&)
{
    throw utils::CriticalError(
        "This merge tree does not support being computed on a subset "
        "of indices for a given factored transition system!");
}

} // namespace downward::merge_and_shrink
