#ifndef DOWNWARD_PLUGINS_PDBS_CANONICAL_PDBS_HEURISTIC_H
#define DOWNWARD_PLUGINS_PDBS_CANONICAL_PDBS_HEURISTIC_H

#include "downward/utils/timer.h"

#include <tuple>

namespace language {
class Context;
}

namespace language::plugins {
class Feature;
class Options;
} // namespace language::plugins

namespace downward::cli::pdbs {

void add_canonical_pdbs_options_to_feature(language::plugins::Feature& feature);

std::tuple<utils::Duration> get_canonical_pdbs_arguments_from_options(
    const language::Context& context,
    const language::plugins::Options& opts);

} // namespace downward::cli::pdbs

#endif
