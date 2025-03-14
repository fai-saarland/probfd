#ifndef PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_H
#define PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_H

#include "probfd/cli/solvers/mdp_heuristic_search_options_fwd.h"

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace probfd::bisimulation {
enum class QuotientState : int;
}

namespace probfd::cli::solvers {

template <typename State, typename Action, bool Fret>
void add_mdp_hs_base_options_to_feature(
    downward::cli::plugins::Feature& feature);

template <typename State, typename Action, bool Fret>
MDPHeuristicSearchBaseArgs<State, Action, Fret>
get_mdp_hs_base_args_from_options(
    const downward::cli::plugins::Options& options);

template <typename State, typename Action, bool Fret>
void add_mdp_hs_options_to_feature(downward::cli::plugins::Feature& feature);

template <typename State, typename Action, bool Fret>
MDPHeuristicSearchArgs<State, Action, Fret>
get_mdp_hs_args_from_options(const downward::cli::plugins::Options& options);

// Explicit instantion declarations
extern template void
add_mdp_hs_base_options_to_feature<State, OperatorID, true>(
    downward::cli::plugins::Feature& feature);
extern template void
add_mdp_hs_base_options_to_feature<State, OperatorID, false>(
    downward::cli::plugins::Feature& feature);
extern template void add_mdp_hs_base_options_to_feature<
    bisimulation::QuotientState,
    OperatorID,
    true>(downward::cli::plugins::Feature& feature);
extern template void add_mdp_hs_base_options_to_feature<
    bisimulation::QuotientState,
    OperatorID,
    false>(downward::cli::plugins::Feature& feature);

extern template MDPHeuristicSearchBaseArgs<State, OperatorID, true>
get_mdp_hs_base_args_from_options<State, OperatorID, true>(
    const downward::cli::plugins::Options& options);
extern template MDPHeuristicSearchBaseArgs<State, OperatorID, false>
get_mdp_hs_base_args_from_options<State, OperatorID, false>(
    const downward::cli::plugins::Options& options);
extern template MDPHeuristicSearchBaseArgs<
    bisimulation::QuotientState,
    OperatorID,
    true>
get_mdp_hs_base_args_from_options<
    bisimulation::QuotientState,
    OperatorID,
    true>(const downward::cli::plugins::Options& options);
extern template MDPHeuristicSearchBaseArgs<
    bisimulation::QuotientState,
    OperatorID,
    false>
get_mdp_hs_base_args_from_options<
    bisimulation::QuotientState,
    OperatorID,
    false>(const downward::cli::plugins::Options& options);

extern template void add_mdp_hs_options_to_feature<State, OperatorID, true>(
    downward::cli::plugins::Feature& feature);
extern template void add_mdp_hs_options_to_feature<State, OperatorID, false>(
    downward::cli::plugins::Feature& feature);
extern template void
add_mdp_hs_options_to_feature<bisimulation::QuotientState, OperatorID, true>(
    downward::cli::plugins::Feature& feature);
extern template void
add_mdp_hs_options_to_feature<bisimulation::QuotientState, OperatorID, false>(
    downward::cli::plugins::Feature& feature);

extern template MDPHeuristicSearchArgs<State, OperatorID, true>
get_mdp_hs_args_from_options<State, OperatorID, true>(
    const downward::cli::plugins::Options& options);
extern template MDPHeuristicSearchArgs<State, OperatorID, false>
get_mdp_hs_args_from_options<State, OperatorID, false>(
    const downward::cli::plugins::Options& options);
extern template MDPHeuristicSearchArgs<
    bisimulation::QuotientState,
    OperatorID,
    true>
get_mdp_hs_args_from_options<bisimulation::QuotientState, OperatorID, true>(
    const downward::cli::plugins::Options& options);
extern template MDPHeuristicSearchArgs<
    bisimulation::QuotientState,
    OperatorID,
    false>
get_mdp_hs_args_from_options<bisimulation::QuotientState, OperatorID, false>(
    const downward::cli::plugins::Options& options);

} // namespace probfd::cli::solvers

#endif // PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_H
