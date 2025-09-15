#ifndef PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_H
#define PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_H

#include "probfd/cli/solvers/mdp_heuristic_search_options_fwd.h"

namespace downward::cli::plugins {
class Feature;
class Options;
} // namespace downward::cli::plugins

namespace probfd::cli::solvers {

template <bool Bisimulation, bool Fret>
void add_mdp_hs_base_options_to_feature(
    downward::cli::plugins::Feature& feature);

template <bool Bisimulation, bool Fret>
MDPHeuristicSearchBaseArgs<Bisimulation, Fret>
get_mdp_hs_base_args_from_options(
    const downward::cli::plugins::Options& options);

template <bool Bisimulation, bool Fret>
void add_mdp_hs_options_to_feature(downward::cli::plugins::Feature& feature);

template <bool Bisimulation, bool Fret>
MDPHeuristicSearchArgs<Bisimulation, Fret>
get_mdp_hs_args_from_options(const downward::cli::plugins::Options& options);

// Explicit instantion declarations
extern template void add_mdp_hs_base_options_to_feature<true, true>(
    downward::cli::plugins::Feature& feature);
extern template void add_mdp_hs_base_options_to_feature<true, false>(
    downward::cli::plugins::Feature& feature);
extern template void add_mdp_hs_base_options_to_feature<false, true>(
    downward::cli::plugins::Feature& feature);
extern template void add_mdp_hs_base_options_to_feature<false, false>(
    downward::cli::plugins::Feature& feature);

extern template MDPHeuristicSearchBaseArgs<true, true>
get_mdp_hs_base_args_from_options<true, true>(
    const downward::cli::plugins::Options& options);
extern template MDPHeuristicSearchBaseArgs<true, false>
get_mdp_hs_base_args_from_options<true, false>(
    const downward::cli::plugins::Options& options);
extern template MDPHeuristicSearchBaseArgs<false, true>
get_mdp_hs_base_args_from_options<false, true>(
    const downward::cli::plugins::Options& options);
extern template MDPHeuristicSearchBaseArgs<false, false>
get_mdp_hs_base_args_from_options<false, false>(
    const downward::cli::plugins::Options& options);

extern template void add_mdp_hs_options_to_feature<true, true>(
    downward::cli::plugins::Feature& feature);
extern template void add_mdp_hs_options_to_feature<true, false>(
    downward::cli::plugins::Feature& feature);
extern template void add_mdp_hs_options_to_feature<false, true>(
    downward::cli::plugins::Feature& feature);
extern template void add_mdp_hs_options_to_feature<false, false>(
    downward::cli::plugins::Feature& feature);

extern template MDPHeuristicSearchArgs<true, true>
get_mdp_hs_args_from_options<true, true>(
    const downward::cli::plugins::Options& options);
extern template MDPHeuristicSearchArgs<true, false>
get_mdp_hs_args_from_options<true, false>(
    const downward::cli::plugins::Options& options);
extern template MDPHeuristicSearchArgs<false, true>
get_mdp_hs_args_from_options<false, true>(
    const downward::cli::plugins::Options& options);
extern template MDPHeuristicSearchArgs<false, false>
get_mdp_hs_args_from_options<false, false>(
    const downward::cli::plugins::Options& options);

} // namespace probfd::cli::solvers

#endif // CLI_SOLVERS_MDP_HEURISTIC_SEARCH_H
