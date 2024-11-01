#ifndef PROBFD_ALGORITHMS_INTERVAL_ITERATION_H
#define PROBFD_ALGORITHMS_INTERVAL_ITERATION_H

#include "probfd/algorithms/topological_value_iteration.h"

#include "probfd/preprocessing/end_component_decomposition.h"
#include "probfd/preprocessing/qualitative_reachability_analysis.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/mdp_algorithm.h"

#include <limits>

/// Namespace dedicated to interval iteration on MaxProb MDPs.
namespace probfd::algorithms::interval_iteration {

struct Statistics {
    preprocessing::ECDStatistics ecd_statistics;
    topological_vi::Statistics tvi_statistics;

    void print(std::ostream& out) const
    {
        tvi_statistics.print(out);
        ecd_statistics.print(out);
    }
};

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
class IntervalIteration : public MDPAlgorithm<State, Action> {
    using Base = typename IntervalIteration::MDPAlgorithm;

    using MDPType = typename Base::MDPType;
    using EvaluatorType = typename Base::EvaluatorType;
    using PolicyType = typename Base::PolicyType;

    using QSystem = quotients::QuotientSystem<State, Action>;
    using QState = quotients::QuotientState<State, Action>;
    using QAction = quotients::QuotientAction<Action>;

    using Decomposer = preprocessing::EndComponentDecomposition<State, Action>;
    using QuotientQRAnalysis =
        preprocessing::QualitativeReachabilityAnalysis<QState, QAction>;
    using QuotientValueIteration =
        topological_vi::TopologicalValueIteration<QState, QAction, true>;

    // Algorithm parameters
    const bool extract_probability_one_states_;

    // Algorithm state
    QuotientQRAnalysis qr_analysis_;
    Decomposer ec_decomposer_;
    QuotientValueIteration vi_;

    Statistics statistics_;

public:
    explicit IntervalIteration(
        bool extract_probability_one_states,
        bool expand_goals);

    Interval solve(
        MDPType& mdp,
        EvaluatorType& heuristic,
        param_type<State> state,
        ProgressReport report,
        double max_time) override;

    std::unique_ptr<PolicyType> compute_policy(
        MDPType& mdp,
        EvaluatorType& heuristic,
        param_type<State> state,
        ProgressReport report,
        double max_time) override;

    void print_statistics(std::ostream& out) const override;

    template <typename ValueStoreT, typename SetLike, typename SetLike2>
    Interval solve(
        MDPType& mdp,
        EvaluatorType& heuristic,
        param_type<State> state,
        ValueStoreT& value_store,
        SetLike& dead_ends,
        SetLike2& one_states,
        double max_time = std::numeric_limits<double>::infinity());

private:
    std::unique_ptr<QSystem> create_quotient(
        MDPType& mdp,
        EvaluatorType& heuristic,
        param_type<State> state,
        utils::CountdownTimer& timer);

    template <typename ValueStoreT, typename SetLike, typename SetLike2>
    Interval mysolve(
        MDPType& mdp,
        EvaluatorType& heuristic,
        param_type<State> state,
        ValueStoreT& value_store,
        SetLike& dead_ends,
        SetLike2& one_states,
        QSystem& sys,
        utils::CountdownTimer& timer);
};

} // namespace probfd::algorithms::interval_iteration

#define GUARD_INCLUDE_PROBFD_ALGORITHMS_INTERVAL_ITERATION_H
#include "probfd/algorithms/interval_iteration_impl.h"
#undef GUARD_INCLUDE_PROBFD_ALGORITHMS_INTERVAL_ITERATION_H

#endif // PROBFD_ALGORITHMS_INTERVAL_ITERATION_H