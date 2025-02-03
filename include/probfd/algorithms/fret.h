#ifndef PROBFD_ALGORITHMS_FRET_H
#define PROBFD_ALGORITHMS_FRET_H

#include "probfd/algorithms/heuristic_search_base.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/progress_report.h"

#if defined(EXPENSIVE_STATISTICS)
#include "downward/utils/timer.h"
#endif

#include <limits>
#include <type_traits>

// Forward Declarations
namespace utils {
class CountdownTimer;
}

/// Namespace dedicated to the Find, Revise, Eliminate Traps (FRET) framework.
namespace probfd::algorithms::fret {

namespace internal {

struct Statistics {
    unsigned long long iterations = 0;
    unsigned long long traps = 0;

#if defined(EXPENSIVE_STATISTICS)
    utils::Timer heuristic_search = utils::Timer(true);
    utils::Timer trap_identification = utils::Timer(true);
    utils::Timer trap_removal = utils::Timer(true);
#endif

    void print(std::ostream& out) const;
};

struct TarjanStateInformation {
    static constexpr unsigned UNDEF = std::numeric_limits<unsigned int>::max();

    unsigned stack_index = UNDEF;
    unsigned lowlink = UNDEF;

    [[nodiscard]]
    bool is_explored() const
    {
        return lowlink != UNDEF;
    }

    [[nodiscard]]
    bool is_on_stack() const
    {
        return stack_index != UNDEF;
    }

    void open(const unsigned x)
    {
        stack_index = x;
        lowlink = x;
    }

    void close() { stack_index = UNDEF; }
};

struct ExplorationInfo {
    ExplorationInfo(StateID state_id, std::vector<StateID> successors)
        : state_id(state_id)
        , successors(std::move(successors))
    {
    }

    StateID state_id;
    std::vector<StateID> successors;
    bool is_leaf = true;
};

template <typename QAction>
struct StackInfo {
    StateID state_id;
    std::vector<QAction> aops;

    template <size_t i>
    friend auto& get(StackInfo& info)
    {
        if constexpr (i == 0) return info.state_id;
        if constexpr (i == 1) return info.aops;
    }

    template <size_t i>
    friend const auto& get(const StackInfo& info)
    {
        if constexpr (i == 0) return info.state_id;
        if constexpr (i == 1) return info.aops;
    }
};

} // namespace internal

/**
 * @brief Implemetation of the Find-Revise-Eliminate-Traps (FRET) framework
 * \cite kolobov:etal:icaps-11 .
 *
 * The FRET framework is a framework designed for Generalized Stochastic
 * Shortest-Path Problems (GSSPs, \cite kolobov:etal:icaps-11). In this
 * scenario, traditional heuristic search algorithms do not converge against an
 * optimal policy due to the existence of traps. FRET interleaves heuristic
 * searches with a trap elimination procedure until no more traps are found.
 * This guarantees that an optimal policy is returned.
 *
 * The two common trap elimination strategies find traps in
 * - The greedy value graph of the MDP, or
 * - The greedy policy graph of the optimal policy returned by the last
 * heuristic search
 *
 * @tparam State - The state type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam StateInfoT - The state info type of the heuristic search algorithm.
 * @tparam GreedyGraphGenerator - The type of the generator used to construct
 * the search graph in which traps are found and eliminated between heuristic
 * searches.
 */
template <
    typename State,
    typename Action,
    typename StateInfoT,
    typename GreedyGraphGenerator>
class FRET : public MDPAlgorithm<State, Action> {
    using Base = typename FRET::MDPAlgorithm;

    using PolicyType = typename Base::PolicyType;
    using MDPType = typename Base::MDPType;
    using EvaluatorType = typename Base::EvaluatorType;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QState = quotients::QuotientState<State, Action>;
    using QAction = quotients::QuotientAction<Action>;
    using QHeuristicSearchAlgorithm = heuristic_search::
        FRETHeuristicSearchAlgorithm<QState, QAction, StateInfoT>;
    using QEvaluator = probfd::Heuristic<QState>;

    using StackInfo = internal::StackInfo<QAction>;

    // Algorithm parameters
    const std::shared_ptr<QHeuristicSearchAlgorithm> base_algorithm_;

    internal::Statistics statistics_;

public:
    explicit FRET(std::shared_ptr<QHeuristicSearchAlgorithm> algorithm);

    std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        EvaluatorType& heuristic,
        param_type<State> state,
        ProgressReport progress,
        double max_time) override;

    Interval solve(
        MDPType& mdp,
        EvaluatorType& heuristic,
        param_type<State> state,
        ProgressReport progress,
        double max_time) override;

    void print_statistics(std::ostream& out) const override;

private:
    Interval solve(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        param_type<QState> state,
        ProgressReport& progress,
        double max_time);

    Interval heuristic_search(
        QuotientSystem& quotient,
        QEvaluator& heuristic,
        param_type<QState> state,
        ProgressReport& progress,
        utils::CountdownTimer& timer);

    bool find_and_remove_traps(
        QuotientSystem& quotient,
        param_type<QState> state,
        utils::CountdownTimer& timer);

    bool push(
        QuotientSystem& quotient,
        std::deque<internal::ExplorationInfo>& queue,
        std::deque<StackInfo>& stack,
        internal::TarjanStateInformation& info,
        StateID state_id,
        unsigned int& unexpanded);
};

template <typename State, typename Action, typename StateInfoT>
class ValueGraph {
    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QState = quotients::QuotientState<State, Action>;
    using QAction = quotients::QuotientAction<Action>;

    using QHeuristicSearchAlgorithm =
        heuristic_search::HeuristicSearchAlgorithm<QState, QAction, StateInfoT>;

    using AlgorithmValueType =
        typename QHeuristicSearchAlgorithm::AlgorithmValueType;

    using QEvaluator = Heuristic<QState>;

    std::unordered_set<StateID> ids_;
    std::vector<Transition<QAction>> opt_transitions_;
    std::vector<AlgorithmValueType> q_values;

public:
    bool get_successors(
        QuotientSystem& quotient,
        QHeuristicSearchAlgorithm& base_algorithm,
        StateID qstate,
        std::vector<QAction>& aops,
        std::vector<StateID>& successors);
};

template <typename State, typename Action, typename StateInfoT>
class PolicyGraph {
    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QState = quotients::QuotientState<State, Action>;
    using QAction = quotients::QuotientAction<Action>;
    using QHeuristicSearchAlgorithm =
        heuristic_search::HeuristicSearchAlgorithm<QState, QAction, StateInfoT>;

    using QEvaluator = Heuristic<QState>;

    Distribution<StateID> t_;

public:
    bool get_successors(
        QuotientSystem& quotient,
        QHeuristicSearchAlgorithm& base_algorithm,
        StateID quotient_state_id,
        std::vector<QAction>& aops,
        std::vector<StateID>& successors);
};

/**
 * @brief Implementation of FRET with trap elimination in the greedy value graph
 * of the MDP.
 *
 * @tparam State - The state type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam StateInfoT - The state info type of the heuristic search algorithm.
 */
template <typename State, typename Action, typename StateInfoT>
using FRETV =
    FRET<State, Action, StateInfoT, ValueGraph<State, Action, StateInfoT>>;

/**
 * @brief Implementation of FRET with trap elimination in the greedy policy
 * graph of the last returned policy.
 *
 * @tparam State - The state type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam StateInfoT - The state info type of the heuristic search algorithm.
 */
template <typename State, typename Action, typename StateInfoT>
using FRETPi =
    FRET<State, Action, StateInfoT, PolicyGraph<State, Action, StateInfoT>>;

} // namespace probfd::algorithms::fret

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_FRET_H
#include "probfd/algorithms/fret_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_FRET_H

#endif // PROBFD_ALGORITHMS_FRET_H
