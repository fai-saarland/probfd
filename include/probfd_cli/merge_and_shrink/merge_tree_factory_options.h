
#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_OPTIONS_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_TREE_FACTORY_OPTIONS_H

#include "downward/utils/logging.h"

#include <tuple>

namespace downward::merge_and_shrink {
class MergeUpdateStrategy;
}

namespace language {
class Context;
}

namespace language::plugins {
class Feature;
class Options;
} // namespace language::plugins

namespace probfd::cli::merge_and_shrink {

extern void
add_merge_tree_factory_options_to_feature(language::plugins::Feature& feature);

extern std::tuple<
    std::shared_ptr<downward::merge_and_shrink::MergeUpdateStrategy>>
get_merge_tree_factory_args_from_options(
    const language::Context& context,
    const language::plugins::Options& options);

} // namespace probfd::cli::merge_and_shrink

#endif
