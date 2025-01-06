#ifndef PROBFD_PLUGINS_SOLVERS_MDP_HEURISTIC_SEARCH_H
#define PROBFD_PLUGINS_SOLVERS_MDP_HEURISTIC_SEARCH_H

#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/naming_conventions.h"

#include "mdp_solver.h"

#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/bisimulation/types.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace probfd::cli::solvers {

template <bool Bisimulation, bool Fret>
using PolicyPickerType = probfd::algorithms::PolicyPicker<
    probfd::solvers::StateType<Bisimulation, Fret>,
    probfd::solvers::ActionType<Bisimulation, Fret>>;

template <bool Bisimulation, bool Fret>
void add_mdp_hs_base_options_to_feature(
    downward::cli::plugins::Feature& feature)
{
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

    add_base_solver_options_to_feature(feature);
}

template <bool Bisimulation, bool Fret>
auto get_mdp_hs_base_args_from_options(
    const downward::cli::plugins::Options& options)
{
    return std::tuple_cat(
        std::make_tuple(
            options.get<bool>("dual_bounds"),
            options.get<std::shared_ptr<PolicyPickerType<Bisimulation, Fret>>>(
                "policy")),
        get_base_solver_args_from_options(options));
}

template <bool Bisimulation, bool Fret>
void add_mdp_hs_options_to_feature(downward::cli::plugins::Feature& feature)
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
auto get_mdp_hs_args_from_options(
    const downward::cli::plugins::Options& options)
{
    if constexpr (Fret) {
        return std::tuple_cat(
            std::make_tuple(options.get<bool>("fret_on_policy")),
            get_mdp_hs_base_args_from_options<Bisimulation, Fret>(options));
    } else {
        return get_mdp_hs_base_args_from_options<Bisimulation, Fret>(options);
    }
}

} // namespace probfd::cli::solvers

#endif
