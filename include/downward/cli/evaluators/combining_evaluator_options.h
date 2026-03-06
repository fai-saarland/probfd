#ifndef DOWNWARD_PLUGINS_EVALUATORS_COMBINING_EVALUATOR_H
#define DOWNWARD_PLUGINS_EVALUATORS_COMBINING_EVALUATOR_H

#include <cstddef>
#include <string>

namespace language::parser {
class InternalFunctionDefinitionBase;
} // namespace language::parser

namespace downward::cli::combining_evaluator {

extern std::size_t add_combining_evaluator_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    const std::string& description,
    std::size_t start_index);

} // namespace downward::cli::combining_evaluator

#endif
