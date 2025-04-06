
#ifndef PROBFD_CLI_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_OPTIONS_H
#define PROBFD_CLI_MERGE_AND_SHRINK_MERGE_STRATEGY_FACTORY_OPTIONS_H

#include "downward/utils/logging.h"

#include <tuple>

namespace downward::cli::plugins {
class Feature;
class Options;
}

namespace probfd::cli::merge_and_shrink {

extern void add_merge_strategy_options_to_feature(
    downward::cli::plugins::Feature& feature);

extern std::tuple<downward::utils::Verbosity>
get_merge_strategy_args_from_options(
    const downward::cli::plugins::Options& options);

}

#endif
