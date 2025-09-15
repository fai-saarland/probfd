#ifndef PROBFD_ALGORITHMS_FRET_FWD_H
#define PROBFD_ALGORITHMS_FRET_FWD_H

#include "probfd/mdp_algorithm_fwd.h"

#include "probfd/concepts.h"

#include <concepts>

// Forward Declarations
namespace probfd::quotients {
template <typename State, typename Action>
struct QuotientState;

template <typename Action>
struct QuotientAction;
} // namespace probfd::quotients

namespace probfd::algorithms::heuristic_search {
template <typename State, typename Action, typename StateInfo>
class HeuristicSearchAlgorithm;
}

/// Namespace dedicated to the Find, Revise, Eliminate Traps (FRET) framework.
namespace probfd::algorithms::fret {

template <typename T>
struct RemoveQTypeS;

template <typename State, typename Action>
struct RemoveQTypeS<quotients::QuotientState<State, Action>> {
    using type = State;
};

template <typename Action>
struct RemoveQTypeS<quotients::QuotientAction<Action>> {
    using type = Action;
};

template <typename T>
using RemoveQType = typename RemoveQTypeS<T>::type;

template <typename T>
concept QuotientHeuristicSearchAlgorithm =
    downward::DerivedFromSpecializationOf<
        T,
        heuristic_search::HeuristicSearchAlgorithm> &&
    downward::Specialization<StateTypeOf<T>, quotients::QuotientState> &&
    downward::Specialization<ActionTypeOf<T>, quotients::QuotientAction>;

template <
    QuotientHeuristicSearchAlgorithm NestedAlgorithm,
    typename GreedyGraphGenerator>
class FRET;

template <QuotientHeuristicSearchAlgorithm NestedAlgorithm>
class ValueGraph;

template <QuotientHeuristicSearchAlgorithm NestedAlgorithm>
class PolicyGraph;

/**
 * @brief Implementation of FRET with trap elimination in the greedy value graph
 * of the MDP.
 *
 * @tparam NestedAlgorithm - The inner heuristic search algorithm.
 */
template <QuotientHeuristicSearchAlgorithm NestedAlgorithm>
using FRETV = FRET<NestedAlgorithm, ValueGraph<NestedAlgorithm>>;

/**
 * @brief Implementation of FRET with trap elimination in the greedy policy
 * graph of the last returned policy.
 *
 * @tparam NestedAlgorithm - The inner heuristic search algorithm.
 */
template <QuotientHeuristicSearchAlgorithm NestedAlgorithm>
using FRETPi = FRET<NestedAlgorithm, PolicyGraph<NestedAlgorithm>>;

} // namespace probfd::algorithms::fret

#endif
