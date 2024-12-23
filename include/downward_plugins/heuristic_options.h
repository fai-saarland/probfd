#ifndef DOWNWARD_PLUGINS_HEURISTIC_H
#define DOWNWARD_PLUGINS_HEURISTIC_H

#include <memory>
#include <string>
#include <tuple>

class AbstractTask;

namespace utils {
enum class Verbosity;
}

namespace downward_plugins::plugins {
class Feature;
class Options;
} // namespace downward_plugins::plugins

namespace downward_plugins {

extern void add_heuristic_options_to_feature(
    plugins::Feature& feature,
    const std::string& description);
extern std::
    tuple<std::shared_ptr<AbstractTask>, bool, std::string, utils::Verbosity>
    get_heuristic_arguments_from_options(const plugins::Options& opts);

} // namespace downward_plugins

#endif
