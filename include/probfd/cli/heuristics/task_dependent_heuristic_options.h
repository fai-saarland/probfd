#ifndef PROBFD_CLI_HEURISTICS_TASK_DEPENDENT_HEURISTIC_OPTIONS_H
#define PROBFD_CLI_HEURISTICS_TASK_DEPENDENT_HEURISTIC_OPTIONS_H

#include <cstddef>

namespace language::parser {
class InternalFunctionDefinitionBase;
} // namespace language::parser

namespace probfd::cli::heuristics {

extern std::size_t add_task_dependent_heuristic_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace probfd::cli::heuristics

#endif // PROBFD_CLI_HEURISTICS_TASK_DEPENDENT_HEURISTIC_OPTIONS_H
