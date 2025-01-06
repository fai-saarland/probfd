#include "downward/merge_and_shrink/merge_tree_factory.h"

#include "downward/merge_and_shrink/merge_tree.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng_options.h"
#include "downward/utils/system.h"

#include <iostream>

using namespace std;

namespace merge_and_shrink {
MergeTreeFactory::MergeTreeFactory(int random_seed, UpdateOption update_option)
    : rng(utils::get_rng(random_seed))
    , update_option(update_option)
{
}

void MergeTreeFactory::dump_options(utils::LogProxy& log) const
{
    log << "Merge tree options: " << endl;
    log << "Type: " << name() << endl;
    log << "Update option: ";
    switch (update_option) {
    case UpdateOption::USE_FIRST: log << "use first"; break;
    case UpdateOption::USE_SECOND: log << "use second"; break;
    case UpdateOption::USE_RANDOM: log << "use random"; break;
    }
    log << endl;
    dump_tree_specific_options(log);
}

unique_ptr<MergeTree> MergeTreeFactory::compute_merge_tree(
    const TaskProxy&,
    const FactoredTransitionSystem&,
    const vector<int>&)
{
    cerr << "This merge tree does not support being computed on a subset "
            "of indices for a given factored transition system!"
         << endl;
    utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
}

} // namespace merge_and_shrink
