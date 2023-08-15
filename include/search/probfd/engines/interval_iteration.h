#ifndef PROBFD_ENGINES_INTERVAL_ITERATION_H
#define PROBFD_ENGINES_INTERVAL_ITERATION_H

#include "probfd/engines/topological_value_iteration.h"

#include "probfd/preprocessing/end_component_decomposition.h"
#include "probfd/preprocessing/qualitative_reachability_analysis.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/engine.h"

#include "downward/utils/collections.h"

#include <limits>

namespace probfd {
namespace engines {

/// Namespace dedicated to interval iteration on MaxProb MDPs.
namespace interval_iteration {

/**
 * @brief Implemention of interval iteration \cite haddad:etal:misc-17.
 *
 * While classical value iteration algorithms converge against the optimal
 * value function in a mathematical sense, it is not clear how a termination
 * condition can be derived that ensures a fixed error bound on the computed
 * value function. Interval iteration remedies this issue by performing two
 * value iterations in parallel, starting from a lower and upper bound
 * respectively, and stopping when both bounds are less than epsilon
 * away from each other.
 *
 * Interval iteration consists of two steps:
 * -# Build the MEC decomposition of the underlying MDP to ensure convergence
 * from any initial value function.
 * -# Performs two value iterations in parallel, one from an initial lower bound
 * and one from an initial upper bound.
 *
 * The respective sequences of value functions are adjacent sequences. Interval
 * iteration stops when the lower and upper bounding value functions are less
 * than epsilon away, ensuring that any of the value functions is at most
 * epsilon away from the optimal value function.
 *
 * @note This implementation outputs the values of the upper bounding value
 * function.
 *
 * @see EndComponentDecomposition
 *
 * @tparam State The state type of the underlying MDP model.
 * @tparam Action The action type of the underlying MDP model.
 */
template <typename State, typename Action>
class IntervalIteration : public MDPEngine<State, Action> {
    using Base = typename IntervalIteration::MDPEngine;

    using MDP = typename Base::MDP;
    using Evaluator = typename Base::Evaluator;

    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QState = quotients::QuotientState<State, Action>;
    using QAction = quotients::QuotientAction<Action>;

    using Decomposer = preprocessing::EndComponentDecomposition<State, Action>;
    using QuotientQRAnalysis =
        preprocessing::QualitativeReachabilityAnalysis<QState, QAction>;
    using QuotientValueIteration =
        topological_vi::TopologicalValueIteration<QState, QAction, true>;

    const bool extract_probability_one_states_;
    const bool expand_goals_;

    QuotientQRAnalysis qr_analysis;
    QuotientValueIteration vi;

    preprocessing::ECDStatistics ecd_statistics_;
    topological_vi::Statistics tvi_statistics_;

    storage::PerStateStorage<Interval> value_store_;

public:
    explicit IntervalIteration(
        bool extract_probability_one_states,
        bool expand_goals);

    Interval solve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        double max_time) override;

    void print_statistics(std::ostream& out) const override;

    template <typename ValueStoreT, typename SetLike, typename SetLike2>
    Interval solve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        ValueStoreT& value_store,
        SetLike& dead_ends,
        SetLike2& one_states,
        double max_time = std::numeric_limits<double>::infinity());

private:
    std::unique_ptr<QuotientSystem> create_quotient(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        utils::CountdownTimer& timer);

    template <typename ValueStoreT, typename SetLike, typename SetLike2>
    Interval mysolve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        ValueStoreT& value_store,
        SetLike& dead_ends,
        SetLike2& one_states,
        QuotientSystem& sys,
        utils::CountdownTimer timer);
};

} // namespace interval_iteration
} // namespace engines
} // namespace probfd

#define GUARD_INCLUDE_PROBFD_ENGINES_INTERVAL_ITERATION_H
#include "probfd/engines/interval_iteration_impl.h"
#undef GUARD_INCLUDE_PROBFD_ENGINES_INTERVAL_ITERATION_H

#endif // __INTERVAL_ITERATION_H__dead_ends