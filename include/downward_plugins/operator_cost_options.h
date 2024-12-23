#ifndef DOWNWARD_PLUGINS_OPERATOR_COST_H
#define DOWNWARD_PLUGINS_OPERATOR_COST_H

#include <tuple>

enum OperatorCost : unsigned short;

namespace downward_plugins::plugins {
class Feature;
class Options;
} // namespace downward_plugins::plugins

namespace downward_plugins {

extern void add_cost_type_options_to_feature(plugins::Feature& feature);
extern std::tuple<OperatorCost>
get_cost_type_arguments_from_options(const plugins::Options& opts);

} // namespace downward_plugins

#endif
