#ifndef PROBFD_CLI_SOLVERS_MDP_SOLVER_H
#define PROBFD_CLI_SOLVERS_MDP_SOLVER_H

#include <cstddef>

namespace language::plugins {
class InternalFunctionDefinitionBase;
} // namespace language::plugins

/// This namespace contains the solver plugins for various search algorithms.
namespace probfd::cli::solvers {

extern std::size_t add_base_solver_options_except_algorithm_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

extern std::size_t add_base_solver_options_to_feature(
    language::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace probfd::cli::solvers

#endif // PROBFD_CLI_SOLVERS_MDP_SOLVER_H
