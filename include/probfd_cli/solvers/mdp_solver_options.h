#ifndef PROBFD_CLI_SOLVERS_MDP_SOLVER_H
#define PROBFD_CLI_SOLVERS_MDP_SOLVER_H

#include "probfd_cli/solvers/mdp_solver_options_fwd.h"

namespace language {
class Context;
}

namespace language::plugins {
class Options;
class Feature;
} // namespace language::plugins

/// This namespace contains the solver plugins for various search algorithms.
namespace probfd::cli::solvers {

extern void add_base_solver_options_except_algorithm_to_feature(
    language::plugins::Feature& feature);

extern void
add_base_solver_options_to_feature(language::plugins::Feature& feature);

extern MDPSolverArgs get_base_solver_args_from_options(
    const language::Context& context,
    const language::plugins::Options& options);

extern MDPSolverNoAlgorithmArgs get_base_solver_args_no_algorithm_from_options(
    const language::Context& context,
    const language::plugins::Options& options);

} // namespace probfd::cli::solvers

#endif // PROBFD_CLI_SOLVERS_MDP_SOLVER_H
