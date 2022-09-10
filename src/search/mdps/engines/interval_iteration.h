#ifndef MDPS_ENGINES_INTERVAL_ITERATION_H
#define MDPS_ENGINES_INTERVAL_ITERATION_H

#include "../end_components/end_component_decomposition.h"
#include "../end_components/qualitative_reachability_analysis.h"
#include "../quotient_system/engine_interfaces.h"
#include "../quotient_system/quotient_system.h"
#include "../storage/per_state_storage.h"
#include "engine.h"
#include "topological_value_iteration.h"

#include <iterator>

namespace probabilistic {
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
public:
    using Decomposer = end_components::EndComponentDecomposition<State, Action>;
    using QuotientSystem = typename Decomposer::QuotientSystem;
    using QAction = typename QuotientSystem::QAction;

    using ValueIteration = topological_vi::
        TopologicalValueIteration<State, QAction, std::true_type>;

    using ValueStore = typename ValueIteration::Store;
    using BoolStore = storage::PerStateStorage<bool>;

    explicit IntervalIteration(
        StateIDMap<State>* state_id_map,
        ActionIDMap<Action>* action_id_map,
        RewardFunction<State, Action>* reward_function,
        value_utils::IntervalValue reward_bound,
        TransitionGenerator<Action>* transition_generator,
        const StateEvaluator<State>* prune,
        bool extract_probability_one_states,
        bool expand_goals)
        : MDPEngine<State, Action>(
              state_id_map,
              action_id_map,
              reward_function,
              reward_bound,
              transition_generator)
        , prune_(prune)
        , extract_probability_one_states_(extract_probability_one_states)
        , expand_goals_(expand_goals)
    {
    }

    virtual value_type::value_t solve(const State& state) override
    {
        BoolStore dead(false);
        BoolStore one(false);
        std::unique_ptr<QuotientSystem> sys = get_quotient(state);
        this->mysolve(state, value_store_, dead, one, sys.get());

        const StateID state_id = this->get_state_id(state);
        return value_utils::as_upper_bound(value_store_[state_id]);
    }

    virtual bool supports_error_bound() const override { return true; }

    virtual value_type::value_t get_error(const State& s) override
    {
        const StateID state_id = this->get_state_id(s);
        return value_store_[state_id].error_bound();
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        tvi_statistics_.print(out);
        ecd_statistics_.print(out);
    }

    template <typename ValueStoreT, typename BoolStoreT, typename BoolStoreT2>
    value_type::value_t solve(
        const State& state,
        ValueStoreT& value_store,
        BoolStoreT& dead_ends,
        BoolStoreT2& one_states)
    {
        auto sys = get_quotient(state);

        value_type::value_t x =
            this->mysolve(state, value_store, dead_ends, one_states, sys.get());
        for (StateID repr_id : *sys) {
            auto [sit, send] = sys->quotient_range(repr_id);
            const StateID repr = *sit;

            const auto value = value_store[repr];
            const bool dead = dead_ends[repr];
            const bool one = one_states[repr];
            for (++sit; sit != send; ++sit) {
                value_store[*sit] = value;
                dead_ends[*sit] = dead;
                one_states[*sit] = one;
            }
        }

        return x;
    }

private:
    template <typename BoolStoreT>
    struct OneRewardFunction
        : public RewardFunction<
              State,
              quotient_system::QuotientAction<Action>> {
        using QAction = quotient_system::QuotientAction<Action>;
        StateIDMap<State>* state_id_map_;
        BoolStoreT& one_states_;
        RewardFunction<State, QAction>& fallback_;

        OneRewardFunction(
            StateIDMap<State>* state_id_map,
            BoolStoreT& one_states,
            RewardFunction<State, QAction>& fallback)
            : state_id_map_(state_id_map)
            , one_states_(one_states)
            , fallback_(fallback)
        {
        }

        virtual EvaluationResult evaluate(const State& s) override
        {
            const StateID id = state_id_map_->get_state_id(s);

            if (one_states_[id]) {
                return EvaluationResult(true, value_type::one);
            }

            return EvaluationResult(false, value_type::zero);
        }

        virtual value_type::value_t evaluate(StateID id, QAction a) override
        {
            return fallback_(id, a);
        }
    };

    template <typename BoolStoreT>
    struct HeuristicWrapper : public StateEvaluator<State> {
        HeuristicWrapper(
            StateIDMap<State>* state_id_map,
            BoolStoreT& dead_ends,
            const StateEvaluator<State>* fallback)
            : state_id_map_(state_id_map)
            , dead_ends_(dead_ends)
            , fallback(fallback)
        {
        }

        EvaluationResult evaluate(const State& s) const override
        {
            const StateID id = state_id_map_->get_state_id(s);

            if (dead_ends_[id]) {
                return EvaluationResult(true, value_type::zero);
            }

            if (fallback) {
                return fallback->operator()(s);
            }

            return EvaluationResult(false, value_type::one);
        }

        StateIDMap<State>* state_id_map_;
        BoolStoreT& dead_ends_;
        const StateEvaluator<State>* fallback;
    };

    std::unique_ptr<QuotientSystem> get_quotient(const State& state)
    {
        Decomposer ec_decomposer(
            this->get_state_id_map(),
            this->get_action_id_map(),
            this->get_reward_function(),
            this->get_transition_generator(),
            expand_goals_,
            prune_);

        auto sys = ec_decomposer.build_quotient_system(state);

        ecd_statistics_ = ec_decomposer.get_statistics();

        return std::move(sys);
    }

    template <typename ValueStoreT, typename BoolStoreT, typename BoolStoreT2>
    value_type::value_t mysolve(
        const State& state,
        ValueStoreT& value_store,
        BoolStoreT& dead_ends,
        BoolStoreT2& one_states,
        QuotientSystem* sys)
    {
        TransitionGenerator<QAction> q_transition_gen(sys);
        quotient_system::DefaultQuotientRewardFunction<State, Action> q_reward(
            sys,
            this->get_reward_function());
        ActionIDMap<QAction> q_action_id_map(sys);

        reachability::QualitativeReachabilityAnalysis<State, QAction> analysis(
            this->get_state_id_map(),
            &q_action_id_map,
            &q_reward,
            &q_transition_gen,
            expand_goals_);

        std::set<StateID> gp_zero_states;
        std::set<StateID> gp_one_states;

        if (extract_probability_one_states_) {
            analysis.run_analysis(
                state,
                std::inserter(gp_zero_states, gp_zero_states.end()),
                std::inserter(gp_one_states, gp_one_states.end()));
        } else {
            analysis.run_analysis(
                state,
                std::inserter(gp_zero_states, gp_zero_states.end()),
                utils::discarding_output_iterator());
        }

        sys->build_quotient(gp_zero_states);
        sys->build_quotient(gp_one_states);

        for (StateID id : gp_zero_states) {
            dead_ends[id] = true;
        }

        for (StateID id : gp_one_states) {
            one_states[id] = true;
        }

        HeuristicWrapper<BoolStoreT> heuristic(
            this->get_state_id_map(),
            dead_ends,
            prune_);

        if (!extract_probability_one_states_) {
            ValueIteration vi(
                this->get_state_id_map(),
                &q_action_id_map,
                &q_reward,
                value_utils::IntervalValue(value_type::zero, value_type::one),
                &q_transition_gen,
                &heuristic,
                expand_goals_);

            value_type::value_t result = vi.solve(state, value_store);
            tvi_statistics_ = vi.get_statistics();
            return result;
        } else {
            OneRewardFunction<BoolStoreT2> reward(
                this->get_state_id_map(),
                one_states,
                q_reward);

            ValueIteration vi(
                this->get_state_id_map(),
                &q_action_id_map,
                &reward,
                value_utils::IntervalValue(value_type::zero, value_type::one),
                &q_transition_gen,
                &heuristic,
                expand_goals_);

            value_type::value_t result = vi.solve(state, value_store);
            tvi_statistics_ = vi.get_statistics();
            return result;
        }
    }

    const StateEvaluator<State>* prune_;
    const bool extract_probability_one_states_;
    const bool expand_goals_;

    end_components::Statistics ecd_statistics_;
    topological_vi::Statistics tvi_statistics_;

    ValueStore value_store_;
};

} // namespace interval_iteration
} // namespace engines
} // namespace probabilistic

#endif // __INTERVAL_ITERATION_H__