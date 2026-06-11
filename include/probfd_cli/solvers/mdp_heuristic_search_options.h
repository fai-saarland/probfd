#ifndef PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_H
#define PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_H

#include "probfd_cli/solvers/mdp_heuristic_search_options_fwd.h"

namespace language {
class Context;
}

namespace language::plugins {
class Feature;
class Options;
} // namespace language::plugins

namespace probfd::cli::solvers {

template <bool Bisimulation, bool Fret>
void add_mdp_hs_base_options_to_feature(language::plugins::Feature& feature);

template <bool Bisimulation, bool Fret>
MDPHeuristicSearchBaseArgs<Bisimulation, Fret>
get_mdp_hs_base_args_from_options(
    const language::Context& context,
    const language::plugins::Options& options);

template <bool Bisimulation, bool Fret>
void add_mdp_hs_options_to_feature(language::plugins::Feature& feature);

template <bool Bisimulation, bool Fret>
MDPHeuristicSearchArgs<Bisimulation, Fret> get_mdp_hs_args_from_options(
    const language::Context& context,
    const language::plugins::Options& options);

// Explicit instantion declarations
extern template void add_mdp_hs_base_options_to_feature<true, true>(
    language::plugins::Feature& feature);
extern template void add_mdp_hs_base_options_to_feature<true, false>(
    language::plugins::Feature& feature);
extern template void add_mdp_hs_base_options_to_feature<false, true>(
    language::plugins::Feature& feature);
extern template void add_mdp_hs_base_options_to_feature<false, false>(
    language::plugins::Feature& feature);

extern template MDPHeuristicSearchBaseArgs<true, true>
get_mdp_hs_base_args_from_options<true, true>(
    const language::Context& context,
    const language::plugins::Options& options);
extern template MDPHeuristicSearchBaseArgs<true, false>
get_mdp_hs_base_args_from_options<true, false>(
    const language::Context& context,
    const language::plugins::Options& options);
extern template MDPHeuristicSearchBaseArgs<false, true>
get_mdp_hs_base_args_from_options<false, true>(
    const language::Context& context,
    const language::plugins::Options& options);
extern template MDPHeuristicSearchBaseArgs<false, false>
get_mdp_hs_base_args_from_options<false, false>(
    const language::Context& context,
    const language::plugins::Options& options);

extern template void
add_mdp_hs_options_to_feature<true, true>(language::plugins::Feature& feature);
extern template void
add_mdp_hs_options_to_feature<true, false>(language::plugins::Feature& feature);
extern template void
add_mdp_hs_options_to_feature<false, true>(language::plugins::Feature& feature);
extern template void add_mdp_hs_options_to_feature<false, false>(
    language::plugins::Feature& feature);

extern template MDPHeuristicSearchArgs<true, true>
get_mdp_hs_args_from_options<true, true>(
    const language::Context& context,
    const language::plugins::Options& options);
extern template MDPHeuristicSearchArgs<true, false>
get_mdp_hs_args_from_options<true, false>(
    const language::Context& context,
    const language::plugins::Options& options);
extern template MDPHeuristicSearchArgs<false, true>
get_mdp_hs_args_from_options<false, true>(
    const language::Context& context,
    const language::plugins::Options& options);
extern template MDPHeuristicSearchArgs<false, false>
get_mdp_hs_args_from_options<false, false>(
    const language::Context& context,
    const language::plugins::Options& options);

} // namespace probfd::cli::solvers

#endif // CLI_SOLVERS_MDP_HEURISTIC_SEARCH_H
