#ifndef PROBFD_ENGINES_FRET_H
#define PROBFD_ENGINES_FRET_H

#include "probfd/engines/heuristic_search_base.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/progress_report.h"

#if defined(EXPENSIVE_STATISTICS)
#include "downward/utils/timer.h"
#endif

#include <limits>
#include <type_traits>

namespace utils {
class CountdownTimer;
}

namespace probfd {
namespace engines {

/// Namespace dedicated to the Find, Revise, Eliminate Traps (FRET) framework.
namespace fret {

template <typename State, typename Action, bool UseInterval>
using HeuristicSearchEngine =
    heuristic_search::HeuristicSearchEngine<State, Action, UseInterval, true>;

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
 * @tparam UseInterval - Whether value intervals are used.
 * @tparam GreedyGraphGenerator - The type of the generator used to construct
 * the search graph in which traps are found and eliminated between heuristic
 * searches.
 */
template <
    typename State,
    typename Action,
    bool UseInterval,
    typename GreedyGraphGenerator>
class FRET : public MDPEngine<State, Action> {
    using Base = typename FRET::MDPEngine;

    using Policy = typename Base::Policy;
    using MDP = typename Base::MDP;
    using Evaluator = typename Base::Evaluator;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QAction = quotients::QuotientAction<Action>;
    using QHeuristicSearchEngine =
        HeuristicSearchEngine<State, QAction, UseInterval>;

    using StackIterator = std::deque<StateID>::iterator;

    struct TarjanStateInformation {
        static constexpr unsigned UNDEF = -1;

        unsigned stack_index = UNDEF;
        unsigned lowlink = UNDEF;

        bool is_explored() const { return lowlink != UNDEF; }
        bool is_on_stack() const { return stack_index != UNDEF; }

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

    std::shared_ptr<QHeuristicSearchEngine> base_engine_;

    Statistics statistics_;

public:
    FRET(
        ProgressReport* report,
        std::shared_ptr<QHeuristicSearchEngine> engine);

    std::unique_ptr<Policy> compute_policy(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        double max_time = std::numeric_limits<double>::infinity()) override;

    Interval solve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        double max_time = std::numeric_limits<double>::infinity()) override;

protected:
    void print_statistics(std::ostream& out) const override;

private:
    Interval solve(
        QuotientSystem& quotient,
        Evaluator& heuristic,
        param_type<State> state,
        double max_time);

    Interval heuristic_search(
        QuotientSystem& quotient,
        Evaluator& heuristic,
        param_type<State> state,
        utils::CountdownTimer& timer);

    bool find_and_remove_traps(
        QuotientSystem& quotient,
        Evaluator& heuristic,
        param_type<State> state,
        utils::CountdownTimer& timer);

    void collapse_trap(QuotientSystem& quotient, auto scc);

    bool push(
        QuotientSystem& quotient,
        Evaluator& heuristic,
        std::deque<ExplorationInfo>& queue,
        std::deque<StackInfo>& stack,
        TarjanStateInformation& info,
        StateID state_id,
        unsigned int& unexpanded);
};

template <typename State, typename Action, bool UseInterval>
class ValueGraph {
    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QAction = quotients::QuotientAction<Action>;
    using QHeuristicSearchEngine =
        HeuristicSearchEngine<State, QAction, UseInterval>;

    using Evaluator = Evaluator<State>;

    std::unordered_set<StateID> ids;
    std::vector<Transition<QAction>> opt_transitions;

public:
    bool get_successors(
        QuotientSystem& quotient,
        Evaluator& heuristic,
        QHeuristicSearchEngine& base_engine,
        StateID qstate,
        std::vector<QAction>& aops,
        std::vector<StateID>& successors);
};

template <typename State, typename Action, bool UseInterval>
class PolicyGraph {
    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QAction = quotients::QuotientAction<Action>;
    using QHeuristicSearchEngine =
        HeuristicSearchEngine<State, QAction, UseInterval>;

    using Evaluator = Evaluator<State>;

    Distribution<StateID> t_;

public:
    bool get_successors(
        QuotientSystem& quotient,
        Evaluator& heuristic,
        QHeuristicSearchEngine& base_engine,
        StateID qstate,
        std::vector<QAction>& aops,
        std::vector<StateID>& successors);
};

/**
 * @brief Implementation of FRET with trap elimination in the greedy value graph
 * of the MDP.
 *
 * @tparam State - The state type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam UseInterval - Whether interval state values are used.
 */
template <typename State, typename Action, bool UseInterval>
using FRETV =
    FRET<State, Action, UseInterval, ValueGraph<State, Action, UseInterval>>;

/**
 * @brief Implementation of FRET with trap elimination in the greedy policy
 * graph of the last returned policy.
 *
 * @tparam State - The state type of the underlying MDP.
 * @tparam Action - The action type of the underlying MDP.
 * @tparam UseInterval - Whether interval state values are used.
 */
template <typename State, typename Action, bool UseInterval>
using FRETPi =
    FRET<State, Action, UseInterval, PolicyGraph<State, Action, UseInterval>>;

} // namespace fret
} // namespace engines
} // namespace probfd

#define GUARD_INCLUDE_PROBFD_ENGINES_FRET_H
#include "probfd/engines/fret_impl.h"
#undef GUARD_INCLUDE_PROBFD_ENGINES_FRET_H

#endif