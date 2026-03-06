#ifndef PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_H
#define PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_H

#include <cstddef>

namespace language::parser {
class InternalFunctionDefinitionBase;
} // namespace language::parser

namespace probfd::cli::solvers {

template <bool Bisimulation, bool Fret>
std::size_t add_mdp_hs_base_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

template <bool Bisimulation, bool Fret>
std::size_t add_mdp_hs_options_to_feature(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

// Explicit instantion declarations
extern template std::size_t add_mdp_hs_base_options_to_feature<true, true>(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
extern template std::size_t add_mdp_hs_base_options_to_feature<true, false>(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
extern template std::size_t add_mdp_hs_base_options_to_feature<false, true>(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
extern template std::size_t add_mdp_hs_base_options_to_feature<false, false>(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

extern template std::size_t add_mdp_hs_options_to_feature<true, true>(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
extern template std::size_t add_mdp_hs_options_to_feature<true, false>(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
extern template std::size_t add_mdp_hs_options_to_feature<false, true>(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
extern template std::size_t add_mdp_hs_options_to_feature<false, false>(
    language::parser::InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace probfd::cli::solvers

#endif // PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_H
