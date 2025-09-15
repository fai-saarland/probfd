#ifndef DOWNWARD_PLUGINS_PDBS_CANONICAL_PDBS_HEURISTIC_H
#define DOWNWARD_PLUGINS_PDBS_CANONICAL_PDBS_HEURISTIC_H

#include "downward/utils/timer.h"

#include <tuple>

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace downward::cli::pdbs {

void add_canonical_pdbs_options_to_feature(plugins::Feature& feature);

std::tuple<utils::Duration>
get_canonical_pdbs_arguments_from_options(const plugins::Options& opts);

} // namespace downward::cli::pdbs

#endif
