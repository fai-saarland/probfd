#ifndef DOWNWARD_PLUGINS_MERGE_AND_SHRINK_SHRINK_BUCKET_BASED_H
#define DOWNWARD_PLUGINS_MERGE_AND_SHRINK_SHRINK_BUCKET_BASED_H

#include <tuple>

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace downward::cli::merge_and_shrink {

extern void add_shrink_bucket_options_to_feature(plugins::Feature& feature);

extern std::tuple<int>
get_shrink_bucket_arguments_from_options(const plugins::Options& opts);

} // namespace downward::cli::merge_and_shrink

#endif
