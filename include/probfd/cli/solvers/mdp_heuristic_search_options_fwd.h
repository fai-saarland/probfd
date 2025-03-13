#ifndef PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_OPTIONS_FWD_H
#define PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_OPTIONS_FWD_H

#include "probfd/aliases.h"

#include "probfd/solvers/mdp_heuristic_search.h"

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
using MDPHeuristicSearchBaseArgs = std::
    tuple<value_t, bool, std::shared_ptr<PolicyPickerType<Bisimulation, Fret>>>;

using FretAdditionalArgs = std::tuple<bool>;

template <bool Bisimulation, bool Fret>
using MDPHeuristicSearchArgs = std::conditional_t<
    Fret,
    TupleCatType<
        FretAdditionalArgs,
        MDPHeuristicSearchBaseArgs<Bisimulation, Fret>>,
    MDPHeuristicSearchBaseArgs<Bisimulation, Fret>>;

} // namespace probfd::cli::solvers

#endif // CLI_SOLVERS_MDP_HEURISTIC_SEARCH_H
