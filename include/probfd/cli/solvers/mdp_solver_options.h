#ifndef PROBFD_CLI_SOLVERS_MDP_SOLVER_H
#define PROBFD_CLI_SOLVERS_MDP_SOLVER_H

#include <cstddef>

namespace language::parser {
class InternalFunctionDefinitionBase;
} // namespace language::parser

/// This namespace contains the solver plugins for various search algorithms.
namespace probfd::cli::solvers {

extern std::size_t add_base_solver_options_except_algorithm_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

extern std::size_t add_base_solver_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace probfd::cli::solvers

#endif // PROBFD_CLI_SOLVERS_MDP_SOLVER_H
