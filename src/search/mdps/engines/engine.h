#ifndef MDPS_ENGINES_ENGINE_H
#define MDPS_ENGINES_ENGINE_H

#include "../distribution.h"
#include "../engine_interfaces/action_reward_function.h"
#include "../engine_interfaces/action_id_map.h"
#include "../engine_interfaces/applicable_actions_generator.h"
#include "../engine_interfaces/state_evaluator.h"
#include "../engine_interfaces/state_id_map.h"
#include "../engine_interfaces/state_reward_function.h"
#include "../engine_interfaces/transition_generator.h"
#include "../value_utils.h"

#include <vector>

namespace probabilistic {

/// Namespace dedicated to MDP engines and algorithms.
namespace engines {

/**
 * @brief Interface for MDP algorithm implementations.
 *
 * @tparam State - The state type of the underlying MDP model.
 */
template <typename State>
class MDPEngineInterface {
public:
    virtual ~MDPEngineInterface() = default;
    virtual void reset_solver_state() {}

    /**
     * @brief Runs the MDP algorithm with the initial state \p state .
     */
    virtual value_type::value_t solve(const State& state) = 0;

    /**
     * @brief Checks if the algorithm enforces an error bound on the computed
     * value(s).
     */
    virtual bool supports_error_bound() const { return false; }

    /**
     * @brief Returns the error bound on the value computed for a state, if the
     * underlying engine supports it. It is an error to call this function if
     * supports_error_bound() returns false.
     *
     * @see supports_error_bound()
     */
    virtual value_type::value_t get_error(const State&) { return 0; }

    /**
     * @brief Prints algorithm statistics to the specified std::ostream.
     */
    virtual void print_statistics(std::ostream&) const {}
};

/**
 * @brief Common base class for MDP algorithm implementations.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename State, typename Action>
class MDPEngine : public MDPEngineInterface<State> {
public:
    /**
     * @brief Construct the MDP engine from the given MDP model interfaces.
     *
     * @param state_id_map - The state id mapping (populated by the engine).
     * @param action_id_map - The action id mapping (populated by the engine).
     * @param state_reward_function - The state reward interface.
     * @param action_reward_function - The action reward interface.
     * @param reward_bound - A bound on reward values.
     * @param maximal_reward - An upper bound on reward values.
     * @param aops_generator - The applicable action generator.
     * @param transition_generator - The transition generator.
     */
    explicit MDPEngine(
        StateIDMap<State>* state_id_map,
        ActionIDMap<Action>* action_id_map,
        StateRewardFunction<State>* state_reward_function,
        ActionRewardFunction<Action>* action_reward_function,
        value_utils::IntervalValue reward_bound,
        ApplicableActionsGenerator<Action>* aops_generator,
        TransitionGenerator<Action>* transition_generator)
        : state_id_map_(state_id_map)
        , action_id_map_(action_id_map)
        , state_reward_function_(state_reward_function)
        , action_reward_function_(action_reward_function)
        , reward_bound_(reward_bound)
        , aops_generator_(aops_generator)
        , transition_generator_(transition_generator)
    {
    }

    /**
     * @brief Looks up the state id of state \p s in the state id mapping.
     */
    StateID get_state_id(const State& s) const
    {
        return state_id_map_->get_state_id(s);
    }

    /**
     * @brief Looks up the state corresponding to id \p sid in the state id
     * mapping.
     */
    State lookup_state(const StateID& sid) const
    {
        return state_id_map_->get_state(sid);
    }

    /**
     * @brief Looks up the action id of action \p a when applied in the state
     * with id \p sid in the action id mapping.
     */
    ActionID get_action_id(const StateID& sid, const Action& a) const
    {
        return action_id_map_->get_action_id(sid, a);
    }

    /**
     * @brief Lookup the action corresponding to action id \p aid when applied
     * in the state with id \p sid in the action id mapping.
     */
    Action lookup_action(const StateID& sid, const ActionID& aid) const
    {
        return action_id_map_->get_action(sid, aid);
    }

    /**
     * @brief Get the state reward for successor state \p s .
     */
    EvaluationResult get_state_reward(const State& s) const
    {
        return state_reward_function_->operator()(s);
    }

    /**
     * @brief Get the action reward for action \p a when applied in the state
     * with id \p sid .
     */
    value_type::value_t
    get_action_reward(const StateID& sid, const Action& a) const
    {
        return action_reward_function_->operator()(sid, a);
    }

    /**
     * @brief Get the lower bound on reward values.
     */
    value_type::value_t get_minimal_reward() const
    {
        return reward_bound_.lower;
    }

    /**
     * @brief Get the upper bound on reward values.
     */
    value_type::value_t get_maximal_reward() const
    {
        return reward_bound_.upper;
    }

    /**
     * @brief Output the list of applicable operators in the state with id
     * \p sid via \p ops.
     */
    void
    generate_applicable_ops(const StateID& sid, std::vector<Action>& ops) const
    {
        aops_generator_->operator()(sid, ops);
    }

    /**
     * @brief Output the successor distribution for the state with id \p sid
     * and applicable action \p a in \p successors .
     */
    void generate_successors(
        const StateID& sid,
        const Action& a,
        Distribution<StateID>& successors) const
    {
        transition_generator_->operator()(sid, a, successors);
    }

    /**
     * @brief Output all applicable actions and successor distributions.
     *
     * @param[in] sid - The source state.
     * @param[out] aops - The list of applicable actions.
     * @param[out] successors - The ordered list of successor distributions for
     * all applicable actions.
     */
    void generate_all_successors(
        const StateID& sid,
        std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors) const
    {
        transition_generator_->operator()(sid, aops, successors);
    }

    /**
     * @brief Get the state id map object.
     */
    StateIDMap<State>* get_state_id_map() const { return state_id_map_; }

    /**
     * @brief Get the action id map object.
     */
    ActionIDMap<Action>* get_action_id_map() const { return action_id_map_; }

    /**
     * @brief Get the state reward interface.
     */
    StateRewardFunction<State>* get_state_reward_function() const
    {
        return state_reward_function_;
    }

    /**
     * @brief Get the action reward interface.
     */
    ActionRewardFunction<Action>* get_action_reward_function() const
    {
        return action_reward_function_;
    }

    /**
     * @brief Get the applicable actions generator.
     */
    ApplicableActionsGenerator<Action>* get_applicable_actions_generator() const
    {
        return aops_generator_;
    }

    /**
     * @brief Get the transition generator.
     */
    TransitionGenerator<Action>* get_transition_generator() const
    {
        return transition_generator_;
    }

private:
    StateIDMap<State>* state_id_map_;
    ActionIDMap<Action>* action_id_map_;
    StateRewardFunction<State>* state_reward_function_;
    ActionRewardFunction<Action>* action_reward_function_;
    const value_utils::IntervalValue reward_bound_;
    ApplicableActionsGenerator<Action>* aops_generator_;
    TransitionGenerator<Action>* transition_generator_;
};

} // namespace engines
} // namespace probabilistic

#endif // __ENGINE_H__