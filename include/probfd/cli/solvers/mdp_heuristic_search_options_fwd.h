#ifndef PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_OPTIONS_FWD_H
#define PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_OPTIONS_FWD_H

#include "probfd/aliases.h"

#include "probfd/solvers/mdp_heuristic_search.h"

#include <memory>
#include <string>
#include <type_traits>
#include <utility>

namespace probfd::cli::solvers {

template <typename State, typename Action, bool Fret>
using PolicyPickerType = probfd::algorithms::PolicyPicker<
    probfd::solvers::StateType<State, Action, Fret>,
    probfd::solvers::ActionType<State, Action, Fret>>;

template <typename State, typename Action, bool Fret>
using MDPHeuristicSearchBaseArgs = std::tuple<
    value_t,
    bool,
    std::shared_ptr<PolicyPickerType<State, Action, Fret>>>;

using FretAdditionalArgs = std::tuple<bool>;

template <typename State, typename Action, bool Fret>
using MDPHeuristicSearchArgs = std::conditional_t<
    Fret,
    TupleCatType<
        FretAdditionalArgs,
        MDPHeuristicSearchBaseArgs<State, Action, Fret>>,
    MDPHeuristicSearchBaseArgs<State, Action, Fret>>;

} // namespace probfd::cli::solvers

#endif // PROBFD_CLI_SOLVERS_MDP_HEURISTIC_SEARCH_OPTIONS_FWD_H
