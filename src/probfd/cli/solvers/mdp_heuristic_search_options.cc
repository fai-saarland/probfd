#include "probfd/cli/solvers/mdp_heuristic_search_options.h"

#include "language/plugins/internal_function_definition.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/bisimulation/types.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

using namespace language::plugins;

namespace probfd::cli::solvers {

template <bool Bisimulation, bool Fret>
std::size_t add_mdp_hs_base_options_to_feature(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    feature.make_optional_argument_with_default(
        start_index,
        "convergence_epsilon",
        "10e-4",
        "The tolerance for convergence checks.");
    feature.make_optional_argument_with_default(
        start_index + 1,
        "dual_bounds",
        "false",
        "Specifies whether the algorithm should use an upper-bounding value "
        "function as well. In this case, convergence checks are made by "
        "comparing whether the lower and upper bounding value functions are "
        "epsilon-close to each other.");
    feature.make_optional_argument_with_default(
        start_index + 2,
        "policy",
        add_mdp_type_to_option<Bisimulation, Fret>(
            "arbitrary_policy_tiebreaker()"),
        "The tie-breaking strategy to use when selecting a greedy policy.");

    return 3;
}

template <bool Bisimulation, bool Fret>
std::size_t add_mdp_hs_options_to_feature(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index)
{
    if constexpr (Fret) {
        feature.make_optional_argument_with_default(
            start_index,
            "fret_on_policy",
            "true",
            "Whether FRET should be used on the greedy policy graph or on the "
            "whole value graph.");

        const auto n = add_mdp_hs_base_options_to_feature<Bisimulation, Fret>(
            feature,
            start_index + 1);

        return n + 1;
    } else {
        return add_mdp_hs_base_options_to_feature<Bisimulation, Fret>(
            feature,
            start_index);
    }
}

template std::size_t add_mdp_hs_base_options_to_feature<true, true>(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
template std::size_t add_mdp_hs_base_options_to_feature<true, false>(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
template std::size_t add_mdp_hs_base_options_to_feature<false, true>(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
template std::size_t add_mdp_hs_base_options_to_feature<false, false>(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

template std::size_t add_mdp_hs_options_to_feature<true, true>(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
template std::size_t add_mdp_hs_options_to_feature<true, false>(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
template std::size_t add_mdp_hs_options_to_feature<false, true>(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index);
template std::size_t add_mdp_hs_options_to_feature<false, false>(
    InternalFunctionDefinitionBase& feature,
    std::size_t start_index);

} // namespace probfd::cli::solvers
