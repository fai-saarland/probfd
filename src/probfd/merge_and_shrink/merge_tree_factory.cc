#include "probfd/merge_and_shrink/merge_tree_factory.h"

#include "probfd/merge_and_shrink/merge_tree.h"

#include "downward/utils/logging.h"
#include "downward/utils/rng_options.h"
#include "downward/utils/system.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/utils/rng_options.h"

#include <iostream>

using namespace std;
using namespace downward::cli::plugins;

namespace probfd::merge_and_shrink {

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
    std::shared_ptr<ProbabilisticTask>&,
    const FactoredTransitionSystem&,
    const vector<int>&)
{
    cerr << "This merge tree does not support being computed on a subset "
            "of indices for a given factored transition system!"
         << endl;
    exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
}

void add_merge_tree_factory_options_to_feature(Feature& feature)
{
    downward::cli::utils::add_rng_options_to_feature(feature);

    feature.add_option<UpdateOption>(
        "update_option",
        "When the merge tree is used within another merge strategy, how "
        "should it be updated when a merge different to a merge from the "
        "tree is performed.",
        "use_random");
}

std::tuple<int, UpdateOption>
get_merge_tree_factory_args_from_options(const Options& options)
{
    return std::tuple_cat(
        downward::cli::utils::get_rng_arguments_from_options(options),
        std::make_tuple(options.get<UpdateOption>("update_option")));
}

namespace {

class MergeTreeFactoryCategoryPlugin
    : public TypedCategoryPlugin<MergeTreeFactory> {
public:
    MergeTreeFactoryCategoryPlugin()
        : TypedCategoryPlugin("PMergeTree")
    {
        document_synopsis(
            "This page describes the available merge trees that can be used to "
            "precompute a merge strategy, either for the entire task or a "
            "given "
            "subset of transition systems of a given factored transition "
            "system.\n"
            "Merge trees are typically used in the merge strategy of type "
            "'precomputed', but they can also be used as fallback merge "
            "strategies in "
            "'combined' merge strategies.");
    }
} _category_plugin;

TypedEnumPlugin<UpdateOption> _enum_plugin(
    {{"use_first",
      "the node representing the index that would have been merged earlier "
      "survives"},
     {"use_second",
      "the node representing the index that would have been merged later "
      "survives"},
     {"use_random", "a random node (of the above two) survives"}});

} // namespace

} // namespace probfd::merge_and_shrink