#ifndef PROBFD_CLI_SOLVERS_MDP_SOLVER_H
#define PROBFD_CLI_SOLVERS_MDP_SOLVER_H

#include "probfd/cli/solvers/mdp_solver_options_fwd.h"

namespace downward::cli::plugins {
class Options;
class Feature;
} // namespace downward::cli::plugins

/// This namespace contains the solver plugins for various search algorithms.
namespace probfd::cli::solvers {

extern void
add_base_solver_options_to_feature(downward::cli::plugins::Feature& feature);

extern MDPSolverArgs get_base_solver_args_from_options(
    const downward::cli::plugins::Options& options);

} // namespace probfd::cli::solvers

#endif // CLI_SOLVERS_MDP_SOLVER_H
