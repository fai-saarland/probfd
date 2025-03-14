#include "probfd/cli/solvers/mdp_heuristic_search.h"

#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_solver.h"

#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/bisimulation/types.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

using namespace downward::cli::plugins;

namespace probfd::cli::solvers {

template <typename State, typename Action, bool Fret>
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
    feature.add_option<std::shared_ptr<PolicyPickerType<State, Action, Fret>>>(
        "policy",
        "The tie-breaking strategy to use when selecting a greedy policy.",
        "arbitrary_policy_tiebreaker()");
}

template <typename State, typename Action, bool Fret>
MDPHeuristicSearchBaseArgs<State, Action, Fret>
get_mdp_hs_base_args_from_options(const Options& options)
{
    return std::tuple_cat(
        std::make_tuple(
            options.get<value_t>("convergence_epsilon"),
            options.get<bool>("dual_bounds"),
            options.get<std::shared_ptr<PolicyPickerType<State, Action, Fret>>>(
                "policy")));
}

template <typename State, typename Action, bool Fret>
void add_mdp_hs_options_to_feature(Feature& feature)
{
    if constexpr (Fret) {
        feature.add_option<bool>(
            "fret_on_policy",
            "Whether FRET should be used on the greedy policy graph or on the "
            "whole value graph.",
            "true");
    }

    add_mdp_hs_base_options_to_feature<State, Action, Fret>(feature);
}

template <typename State, typename Action, bool Fret>
MDPHeuristicSearchArgs<State, Action, Fret>
get_mdp_hs_args_from_options(const Options& options)
{
    if constexpr (Fret) {
        return std::tuple_cat(
            std::make_tuple(options.get<bool>("fret_on_policy")),
            get_mdp_hs_base_args_from_options<State, Action, Fret>(options));
    } else {
        return get_mdp_hs_base_args_from_options<State, Action, Fret>(options);
    }
}

template void
add_mdp_hs_base_options_to_feature<State, OperatorID, true>(Feature& feature);
template void
add_mdp_hs_base_options_to_feature<State, OperatorID, false>(Feature& feature);
template void add_mdp_hs_base_options_to_feature<
    bisimulation::QuotientState,
    OperatorID,
    true>(Feature& feature);
template void add_mdp_hs_base_options_to_feature<
    bisimulation::QuotientState,
    OperatorID,
    false>(Feature& feature);

template MDPHeuristicSearchBaseArgs<State, OperatorID, true>
get_mdp_hs_base_args_from_options<State, OperatorID, true>(
    const Options& options);
template MDPHeuristicSearchBaseArgs<State, OperatorID, false>
get_mdp_hs_base_args_from_options<State, OperatorID, false>(
    const Options& options);
template MDPHeuristicSearchBaseArgs<
    bisimulation::QuotientState,
    OperatorID,
    true>
get_mdp_hs_base_args_from_options<
    bisimulation::QuotientState,
    OperatorID,
    true>(const Options& options);
template MDPHeuristicSearchBaseArgs<
    bisimulation::QuotientState,
    OperatorID,
    false>
get_mdp_hs_base_args_from_options<
    bisimulation::QuotientState,
    OperatorID,
    false>(const Options& options);

template void
add_mdp_hs_options_to_feature<State, OperatorID, true>(Feature& feature);
template void
add_mdp_hs_options_to_feature<State, OperatorID, false>(Feature& feature);
template void
add_mdp_hs_options_to_feature<bisimulation::QuotientState, OperatorID, true>(
    Feature& feature);
template void
add_mdp_hs_options_to_feature<bisimulation::QuotientState, OperatorID, false>(
    Feature& feature);

template MDPHeuristicSearchArgs<State, OperatorID, true>
get_mdp_hs_args_from_options<State, OperatorID, true>(const Options& options);
template MDPHeuristicSearchArgs<State, OperatorID, false>
get_mdp_hs_args_from_options<State, OperatorID, false>(const Options& options);
template MDPHeuristicSearchArgs<bisimulation::QuotientState, OperatorID, true>
get_mdp_hs_args_from_options<bisimulation::QuotientState, OperatorID, true>(
    const Options& options);
template MDPHeuristicSearchArgs<bisimulation::QuotientState, OperatorID, false>
get_mdp_hs_args_from_options<bisimulation::QuotientState, OperatorID, false>(
    const Options& options);

} // namespace probfd::cli::solvers
