#ifndef PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_H
#define PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_H

#include <cstddef>

namespace downward::cli::plugins {
class InternalFunctionDefinitionBase;
} // namespace downward::cli::plugins

namespace probfd::cli::solvers {

template <bool Bisimulation, bool Fret>
std::size_t add_mdp_hs_base_options_to_feature(
    downward::cli::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

template <bool Bisimulation, bool Fret>
std::size_t add_mdp_hs_options_to_feature(
    downward::cli::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

// Explicit instantion declarations
extern template std::size_t add_mdp_hs_base_options_to_feature<true, true>(
    downward::cli::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
extern template std::size_t add_mdp_hs_base_options_to_feature<true, false>(
    downward::cli::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
extern template std::size_t add_mdp_hs_base_options_to_feature<false, true>(
    downward::cli::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
extern template std::size_t add_mdp_hs_base_options_to_feature<false, false>(
    downward::cli::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

extern template std::size_t add_mdp_hs_options_to_feature<true, true>(
    downward::cli::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
extern template std::size_t add_mdp_hs_options_to_feature<true, false>(
    downward::cli::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
extern template std::size_t add_mdp_hs_options_to_feature<false, true>(
    downward::cli::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
extern template std::size_t add_mdp_hs_options_to_feature<false, false>(
    downward::cli::plugins::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace probfd::cli::solvers

#endif // PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_H
