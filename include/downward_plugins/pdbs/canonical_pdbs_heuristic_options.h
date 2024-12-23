#ifndef DOWNWARD_PLUGINS_PDBS_CANONICAL_PDBS_HEURISTIC_H
#define DOWNWARD_PLUGINS_PDBS_CANONICAL_PDBS_HEURISTIC_H

#include <tuple>

namespace downward_plugins::plugins {
class Feature;
class Options;
} // namespace downward_plugins::plugins

namespace downward_plugins::pdbs {

void add_canonical_pdbs_options_to_feature(plugins::Feature& feature);

std::tuple<double>
get_canonical_pdbs_arguments_from_options(const plugins::Options& opts);

} // namespace downward_plugins::pdbs

#endif
