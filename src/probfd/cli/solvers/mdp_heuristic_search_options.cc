#include "probfd/cli/solvers/mdp_heuristic_search_options.h"

#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/bisimulation/types.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

using namespace downward::cli::plugins;

namespace probfd::cli::solvers {

template <bool Bisimulation, bool Fret>
void add_mdp_hs_base_options_to_feature(Feature& feature)
{
    feature.add_option<value_t>(
        "convergence_epsilon",
        "The tolerance for convergence checks.",
        "10e-4");
    feature.add_option<bool>(
        "dual_bounds",
        "Specifies whether the algorithm should use an upper-bounding value "
        "function as well. In this case, convergence checks are made by "
        "comparing whether the lower and upper bounding value functions are "
        "epsilon-close to each other.",
        "false");
    feature.add_option<std::shared_ptr<PolicyPickerType<Bisimulation, Fret>>>(
        "policy",
        "The tie-breaking strategy to use when selecting a greedy policy.",
        add_mdp_type_to_option<Bisimulation, Fret>(
            "arbitrary_policy_tiebreaker()"));
}

template <bool Bisimulation, bool Fret>
MDPHeuristicSearchBaseArgs<Bisimulation, Fret>
get_mdp_hs_base_args_from_options(const Options& options)
{
    return std::tuple_cat(
        std::make_tuple(
            options.get<value_t>("convergence_epsilon"),
            options.get<bool>("dual_bounds"),
            options.get<std::shared_ptr<PolicyPickerType<Bisimulation, Fret>>>(
                "policy")));
}

template <bool Bisimulation, bool Fret>
void add_mdp_hs_options_to_feature(Feature& feature)
{
    if constexpr (Fret) {
        feature.add_option<bool>(
            "fret_on_policy",
            "Whether FRET should be used on the greedy policy graph or on the "
            "whole value graph.",
            "true");
    }

    add_mdp_hs_base_options_to_feature<Bisimulation, Fret>(feature);
}

template <bool Bisimulation, bool Fret>
MDPHeuristicSearchArgs<Bisimulation, Fret>
get_mdp_hs_args_from_options(const Options& options)
{
    if constexpr (Fret) {
        return std::tuple_cat(
            std::make_tuple(options.get<bool>("fret_on_policy")),
            get_mdp_hs_base_args_from_options<Bisimulation, Fret>(options));
    } else {
        return get_mdp_hs_base_args_from_options<Bisimulation, Fret>(options);
    }
}

template void add_mdp_hs_base_options_to_feature<true, true>(Feature& feature);
template void add_mdp_hs_base_options_to_feature<true, false>(Feature& feature);
template void add_mdp_hs_base_options_to_feature<false, true>(Feature& feature);
template void
add_mdp_hs_base_options_to_feature<false, false>(Feature& feature);

template MDPHeuristicSearchBaseArgs<true, true>
get_mdp_hs_base_args_from_options<true, true>(const Options& options);
template MDPHeuristicSearchBaseArgs<true, false>
get_mdp_hs_base_args_from_options<true, false>(const Options& options);
template MDPHeuristicSearchBaseArgs<false, true>
get_mdp_hs_base_args_from_options<false, true>(const Options& options);
template MDPHeuristicSearchBaseArgs<false, false>
get_mdp_hs_base_args_from_options<false, false>(const Options& options);

template void add_mdp_hs_options_to_feature<true, true>(Feature& feature);
template void add_mdp_hs_options_to_feature<true, false>(Feature& feature);
template void add_mdp_hs_options_to_feature<false, true>(Feature& feature);
template void add_mdp_hs_options_to_feature<false, false>(Feature& feature);

template MDPHeuristicSearchArgs<true, true>
get_mdp_hs_args_from_options<true, true>(const Options& options);
template MDPHeuristicSearchArgs<true, false>
get_mdp_hs_args_from_options<true, false>(const Options& options);
template MDPHeuristicSearchArgs<false, true>
get_mdp_hs_args_from_options<false, true>(const Options& options);
template MDPHeuristicSearchArgs<false, false>
get_mdp_hs_args_from_options<false, false>(const Options& options);

} // namespace probfd::cli::solvers
