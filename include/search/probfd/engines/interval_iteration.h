#ifndef MDPS_ENGINES_INTERVAL_ITERATION_H
#define MDPS_ENGINES_INTERVAL_ITERATION_H

#include "probfd/engines/engine.h"
#include "probfd/engines/topological_value_iteration.h"

#include "probfd/preprocessing/end_component_decomposition.h"
#include "probfd/preprocessing/qualitative_reachability_analysis.h"

#include "probfd/quotients/engine_interfaces.h"
#include "probfd/quotients/quotient_system.h"

#include "probfd/storage/per_state_storage.h"

#include "utils/collections.h"

#include <iterator>

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
public:
    using Decomposer = preprocessing::EndComponentDecomposition<State, Action>;
    using QuotientSystem = typename Decomposer::QuotientSystem;
    using QAction = typename QuotientSystem::QAction;

    using ValueIteration =
        topological_vi::TopologicalValueIteration<State, QAction, true>;

    using ValueStore = typename ValueIteration::Store;
    using BoolStore = std::vector<StateID>;

    explicit IntervalIteration(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<Action>* action_id_map,
        engine_interfaces::RewardFunction<State, Action>* reward_function,
        engine_interfaces::TransitionGenerator<Action>* transition_generator,
        const engine_interfaces::StateEvaluator<State>* prune,
        bool extract_probability_one_states,
        bool expand_goals)
        : MDPEngine<State, Action>(
              state_id_map,
              action_id_map,
              reward_function,
              transition_generator)
        , prune_(prune)
        , extract_probability_one_states_(extract_probability_one_states)
        , expand_goals_(expand_goals)
    {
    }

    virtual value_t solve(const State& state) override
    {
        std::unique_ptr<QuotientSystem> sys = get_quotient(state);
        BoolStore dead, one;
        this->mysolve(state, value_store_, dead, one, sys.get());

        const StateID state_id = this->get_state_id(state);
        return as_upper_bound(value_store_[state_id]);
    }

    virtual std::optional<value_t> get_error(const State& s) override
    {
        const StateID state_id = this->get_state_id(s);
        return value_store_[state_id].length();
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        tvi_statistics_.print(out);
        ecd_statistics_.print(out);
    }

    template <typename ValueStoreT, typename SetLike, typename SetLike2>
    value_t solve(
        const State& state,
        ValueStoreT& value_store,
        SetLike& dead_ends,
        SetLike2& one_states)
    {
        auto sys = get_quotient(state);

        value_t x =
            this->mysolve(state, value_store, dead_ends, one_states, sys.get());
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

        return sys;
    }

    template <typename ValueStoreT, typename SetLike, typename SetLike2>
    value_t mysolve(
        const State& state,
        ValueStoreT& value_store,
        SetLike& dead_ends,
        SetLike2& one_states,
        QuotientSystem* sys)
    {
        using namespace engine_interfaces;

        TransitionGenerator<QAction> q_transition_gen(sys);
        quotients::DefaultQuotientRewardFunction<State, Action> q_reward(
            sys,
            this->get_reward_function());
        ActionIDMap<QAction> q_action_id_map(sys);

        preprocessing::QualitativeReachabilityAnalysis<State, QAction> analysis(
            this->get_state_id_map(),
            &q_action_id_map,
            &q_reward,
            &q_transition_gen,
            expand_goals_);

        if (extract_probability_one_states_) {
            analysis.run_analysis(
                state,
                std::back_inserter(dead_ends),
                std::back_inserter(one_states));
            assert(
                this->get_state_reward(this->lookup_state(one_states.front()))
                    .is_goal_state());
        } else {
            analysis.run_analysis(
                state,
                std::back_inserter(dead_ends),
                utils::discarding_output_iterator());
        }

        assert(::utils::is_unique(dead_ends) && ::utils::is_unique(one_states));

        sys->build_quotient(dead_ends);
        sys->build_quotient(one_states);

        const auto new_init_id =
            sys->translate_state_id(this->get_state_id(state));

        ValueIteration vi(
            this->get_state_id_map(),
            &q_action_id_map,
            &q_reward,
            &q_transition_gen,
            prune_,
            expand_goals_);

        value_t result = vi.solve(new_init_id, value_store);
        tvi_statistics_ = vi.get_statistics();
        return result;
    }

    const engine_interfaces::StateEvaluator<State>* prune_;
    const bool extract_probability_one_states_;
    const bool expand_goals_;

    preprocessing::ECDStatistics ecd_statistics_;
    topological_vi::Statistics tvi_statistics_;

    ValueStore value_store_;
};

} // namespace interval_iteration
} // namespace engines
} // namespace probfd

#endif // __INTERVAL_ITERATION_H__dead_ends