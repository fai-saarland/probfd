#ifndef DOWNWARD_PLUGINS_MERGE_AND_SHRINK_SHRINK_BUCKET_BASED_H
#define DOWNWARD_PLUGINS_MERGE_AND_SHRINK_SHRINK_BUCKET_BASED_H

#include <tuple>

namespace language {
class Context;
}

namespace language::plugins {
class Feature;
class Options;
} // namespace language::plugins

namespace downward::cli::merge_and_shrink {

extern void
add_shrink_bucket_options_to_feature(language::plugins::Feature& feature);

extern std::tuple<int> get_shrink_bucket_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace downward::cli::merge_and_shrink

#endif
