#ifndef PROBFD_ENGINES_INTERVAL_ITERATION_H
#define PROBFD_ENGINES_INTERVAL_ITERATION_H

#include "probfd/engines/topological_value_iteration.h"

#include "probfd/preprocessing/end_component_decomposition.h"
#include "probfd/preprocessing/qualitative_reachability_analysis.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/engine.h"

#include "downward/utils/collections.h"

#include <iterator>
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
    using QAction = quotients::QuotientAction<Action>;

    using Decomposer = preprocessing::EndComponentDecomposition<State, Action>;
    using QuotientQRAnalysis =
        preprocessing::QualitativeReachabilityAnalysis<State, QAction>;
    using QuotientValueIteration =
        topological_vi::TopologicalValueIteration<State, QAction, true>;

    const bool extract_probability_one_states_;
    const bool expand_goals_;

    QuotientQRAnalysis qr_analysis;
    QuotientValueIteration vi;

    preprocessing::ECDStatistics ecd_statistics_;
    topological_vi::Statistics tvi_statistics_;

    storage::PerStateStorage<Interval> value_store_;

public:
    using BoolStore = std::vector<StateID>;

    explicit IntervalIteration(
        bool extract_probability_one_states,
        bool expand_goals)
        : extract_probability_one_states_(extract_probability_one_states)
        , expand_goals_(expand_goals)
        , qr_analysis(expand_goals)
        , vi(expand_goals)
    {
    }

    Interval solve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        double max_time) override
    {
        utils::CountdownTimer timer(max_time);
        std::unique_ptr sys = get_quotient(mdp, heuristic, state, timer);
        BoolStore dead, one;
        return mysolve(
            mdp,
            heuristic,
            state,
            value_store_,
            dead,
            one,
            *sys,
            timer);
    }

    void print_statistics(std::ostream& out) const override
    {
        tvi_statistics_.print(out);
        ecd_statistics_.print(out);
    }

    template <typename ValueStoreT, typename SetLike, typename SetLike2>
    Interval solve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        ValueStoreT& value_store,
        SetLike& dead_ends,
        SetLike2& one_states,
        double max_time = std::numeric_limits<double>::infinity())
    {
        utils::CountdownTimer timer(max_time);

        auto sys = get_quotient(mdp, heuristic, state, timer);

        const Interval x = this->mysolve(
            mdp,
            heuristic,
            state,
            value_store,
            dead_ends,
            one_states,
            *sys,
            timer);

        for (StateID repr_id : *sys) {
            auto [sit, send] = sys->quotient_range(repr_id);
            const StateID repr = *sit;

            const auto value = value_store[repr];
            const bool dead = utils::contains(dead_ends, repr);
            const bool one = utils::contains(one_states, repr);
            for (++sit; sit != send; ++sit) {
                value_store[*sit] = value;
                if (dead) dead_ends.push_back(*sit);
                if (one) one_states.push_back(*sit);
            }
        }

        return x;
    }

private:
    std::unique_ptr<QuotientSystem> get_quotient(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        utils::CountdownTimer& timer)
    {
        Decomposer ec_decomposer(expand_goals_, &heuristic);

        auto sys = ec_decomposer.build_quotient_system(
            mdp,
            state,
            timer.get_remaining_time());

        ecd_statistics_ = ec_decomposer.get_statistics();

        return sys;
    }

    template <typename ValueStoreT, typename SetLike, typename SetLike2>
    Interval mysolve(
        MDP& mdp,
        Evaluator& heuristic,
        param_type<State> state,
        ValueStoreT& value_store,
        SetLike& dead_ends,
        SetLike2& one_states,
        QuotientSystem& sys,
        utils::CountdownTimer timer)
    {
        if (extract_probability_one_states_) {
            qr_analysis.run_analysis(
                sys,
                state,
                std::back_inserter(dead_ends),
                iterators::discarding_output_iterator(),
                std::back_inserter(one_states),
                timer.get_remaining_time());
            assert(mdp.get_termination_info(mdp.get_state(one_states.front()))
                       .is_goal_state());
        } else {
            qr_analysis.run_analysis(
                sys,
                state,
                std::back_inserter(dead_ends),
                iterators::discarding_output_iterator(),
                iterators::discarding_output_iterator(),
                timer.get_remaining_time());
        }

        assert(::utils::is_unique(dead_ends) && ::utils::is_unique(one_states));

        sys.build_quotient(dead_ends);
        sys.build_quotient(one_states);

        const auto new_init_id =
            sys.translate_state_id(mdp.get_state_id(state));

        const Interval result = vi.solve(
            sys,
            heuristic,
            new_init_id,
            value_store,
            timer.get_remaining_time());
        tvi_statistics_ = vi.get_statistics();
        return result;
    }
};

} // namespace interval_iteration
} // namespace engines
} // namespace probfd

#endif // __INTERVAL_ITERATION_H__dead_ends