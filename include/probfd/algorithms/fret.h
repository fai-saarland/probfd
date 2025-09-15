#ifndef PROBFD_ALGORITHMS_FRET_H
#define PROBFD_ALGORITHMS_FRET_H

#include "probfd/algorithms/fret_fwd.h"

#include "probfd/algorithms/heuristic_search_base.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/progress_report.h"

#if defined(EXPENSIVE_STATISTICS)
#include "downward/utils/timer.h"
#endif

#include <limits>
#include <type_traits>

// Forward Declarations
namespace downward::utils {
class CountdownTimer;
}

/// Namespace dedicated to the Find, Revise, Eliminate Traps (FRET) framework.
namespace probfd::algorithms::fret {

namespace internal {

struct Statistics {
    unsigned long long iterations = 0;
    unsigned long long traps = 0;

#if defined(EXPENSIVE_STATISTICS)
    utils::Timer heuristic_search = utils::Timer(false);
    utils::Timer trap_identification = utils::Timer(false);
    utils::Timer trap_removal = utils::Timer(false);
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
 * @brief Specialization of the Find-Revise-Eliminate-Traps (FRET) framework
 * \cite kolobov:etal:icaps-11 for algorithms deriving from HeuristicSearchBase.
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
 * @tparam NestedAlgorithm - The inner heuristic search algorithm.
 * @tparam GreedyGraphGenerator - The type of the generator used to construct
 * the search graph in which traps are found and eliminated between heuristic
 * searches.
 */
template <
    QuotientHeuristicSearchAlgorithm NestedAlgorithm,
    typename GreedyGraphGenerator>
class FRET
    : public MDPAlgorithm<
          RemoveQType<StateTypeOf<NestedAlgorithm>>,
          RemoveQType<ActionTypeOf<NestedAlgorithm>>> {
    using QState = StateTypeOf<NestedAlgorithm>;
    using QAction = ActionTypeOf<NestedAlgorithm>;
    using State = RemoveQType<QState>;
    using Action = RemoveQType<QAction>;

    using Base = typename FRET::MDPAlgorithm;

    using PolicyType = typename Base::PolicyType;
    using MDPType = typename Base::MDPType;
    using HeuristicType = typename Base::HeuristicType;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;

    using QHeuristic = probfd::Heuristic<QState>;

    using StackInfo = internal::StackInfo<QAction>;

    // Algorithm parameters
    NestedAlgorithm base_algorithm_;

    internal::Statistics statistics_;

public:
    template <typename... Args>
        requires std::constructible_from<NestedAlgorithm, Args...>
    explicit FRET(Args&&... args);

    std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> state,
        ProgressReport progress,
        downward::utils::Duration max_time) override;

    Interval solve(
        MDPType& mdp,
        HeuristicType& heuristic,
        ParamType<State> state,
        ProgressReport progress,
        downward::utils::Duration max_time);

    void print_statistics(std::ostream& out) const override;

private:
    Interval solve(
        QuotientSystem& quotient,
        QHeuristic& heuristic,
        ParamType<QState> state,
        ProgressReport& progress,
        downward::utils::Duration max_time);

    Interval heuristic_search(
        QuotientSystem& quotient,
        QHeuristic& heuristic,
        ParamType<QState> state,
        ProgressReport& progress,
        downward::utils::CountdownTimer& timer);

    bool find_and_remove_traps(
        QuotientSystem& quotient,
        ParamType<QState> state,
        downward::utils::CountdownTimer& timer);

    bool push(
        QuotientSystem& quotient,
        std::deque<internal::ExplorationInfo>& queue,
        std::deque<StackInfo>& stack,
        internal::TarjanStateInformation& info,
        StateID state_id,
        unsigned int& unexpanded);
};

template <QuotientHeuristicSearchAlgorithm NestedAlgorithm>
class ValueGraph {
    using QState = StateTypeOf<NestedAlgorithm>;
    using QAction = ActionTypeOf<NestedAlgorithm>;
    using State = RemoveQType<QState>;
    using Action = RemoveQType<QAction>;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;

    using AlgorithmValueType =
        typename NestedAlgorithm::HeuristicSearchBase::AlgorithmValueType;

    std::unordered_set<StateID> ids_;
    std::vector<TransitionTail<QAction>> opt_transitions_;
    std::vector<AlgorithmValueType> q_values;

public:
    bool get_successors(
        QuotientSystem& quotient,
        NestedAlgorithm& base_algorithm,
        StateID qstate,
        std::vector<QAction>& aops,
        std::vector<StateID>& successors);
};

template <QuotientHeuristicSearchAlgorithm NestedAlgorithm>
class PolicyGraph {
    using QState = StateTypeOf<NestedAlgorithm>;
    using QAction = ActionTypeOf<NestedAlgorithm>;
    using State = RemoveQType<QState>;
    using Action = RemoveQType<QAction>;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;

    SuccessorDistribution t_;

public:
    bool get_successors(
        QuotientSystem& quotient,
        NestedAlgorithm& base_algorithm,
        StateID quotient_state_id,
        std::vector<QAction>& aops,
        std::vector<StateID>& successors);
};

} // namespace probfd::algorithms::fret

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_FRET_H
#include "probfd/algorithms/fret_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_FRET_H

#endif // PROBFD_ALGORITHMS_FRET_H
