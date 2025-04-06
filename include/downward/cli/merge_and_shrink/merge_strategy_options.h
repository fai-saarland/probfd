#ifndef DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_H
#define DOWNWARD_PLUGINS_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_H

#include <tuple>

namespace downward::utils {
enum class Verbosity;
}

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace downward::cli::merge_and_shrink {

extern void add_merge_strategy_options_to_feature(plugins::Feature& feature);
extern std::tuple<utils::Verbosity>
get_merge_strategy_arguments_from_options(const plugins::Options& opts);

} // namespace downward::cli::merge_and_shrink

#endif
