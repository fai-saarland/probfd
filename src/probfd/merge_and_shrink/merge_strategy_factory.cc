#include "probfd/merge_and_shrink/merge_strategy_factory.h"

#include "downward/cli/plugins/plugin.h"

#include "downward/cli/utils/logging_options.h"

using namespace std;
using namespace downward::cli::plugins;

namespace probfd::merge_and_shrink {

MergeStrategyFactory::MergeStrategyFactory(utils::Verbosity verbosity)
    : log(get_log_for_verbosity(verbosity))
{
}

void MergeStrategyFactory::dump_options() const
{
    if (log.is_at_least_normal()) {
        log << "Merge strategy options:" << endl;
        log << "Type: " << name() << endl;
        dump_strategy_specific_options();
    }
}

void add_merge_strategy_options_to_feature(Feature& feature)
{
    downward::cli::utils::add_log_options_to_feature(feature);
}

std::tuple<::utils::Verbosity>
get_merge_strategy_args_from_options(const Options& options)
{
    return downward::cli::utils::get_log_arguments_from_options(options);
}

namespace {
class MergeStrategyFactoryCategoryPlugin
    : public TypedCategoryPlugin<MergeStrategyFactory> {
public:
    MergeStrategyFactoryCategoryPlugin()
        : TypedCategoryPlugin("PMergeStrategy")
    {
        document_synopsis(
            "This page describes the various merge strategies supported "
            "by the planner.");
    }
} _category_plugin;

} // namespace

} // namespace probfd::merge_and_shrink
