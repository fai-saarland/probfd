#ifndef DOWNWARD_PLUGINS_OPERATOR_COST_H
#define DOWNWARD_PLUGINS_OPERATOR_COST_H

#include <tuple>

namespace downward {
enum OperatorCost : unsigned short;
}

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace downward::cli {

extern void add_cost_type_options_to_feature(plugins::Feature& feature);
extern std::tuple<OperatorCost>
get_cost_type_arguments_from_options(const plugins::Options& opts);

} // namespace downward::cli

#endif
