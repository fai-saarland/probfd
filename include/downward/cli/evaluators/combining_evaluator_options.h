#ifndef DOWNWARD_PLUGINS_EVALUATORS_COMBINING_EVALUATOR_H
#define DOWNWARD_PLUGINS_EVALUATORS_COMBINING_EVALUATOR_H

#include <cstddef>
#include <string>

namespace language::plugins {
class InternalFunctionDefinitionBase;
} // namespace language::plugins

namespace downward::cli::combining_evaluator {

extern std::size_t add_combining_evaluator_options_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    const std::string& description,
    std::size_t start_index);

} // namespace downward::cli::combining_evaluator

#endif
