#ifndef MDPS_ENGINES_ENGINE_H
#define MDPS_ENGINES_ENGINE_H

#include "probfd/engine_interfaces/action_id_map.h"
#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/state_evaluator.h"
#include "probfd/engine_interfaces/state_id_map.h"
#include "probfd/engine_interfaces/transition_generator.h"

#include "probfd/distribution.h"
#include "probfd/value_utils.h"

#include <optional>
#include <vector>

namespace probfd {

/// This namespace contains implementations of SSP search algorithms.
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
    virtual value_t solve(const State& state) = 0;

    /**
     * @brief Returns the error bound on the value computed for a state, if the
     * underlying engine supports it.
     */
    virtual std::optional<value_t> get_error(const State&)
    {
        return std::nullopt;
    }

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
     * @param transition_generator - The transition generator.
     * @param cost_function - The cost function.
     */
    explicit MDPEngine(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<Action>* action_id_map,
        engine_interfaces::TransitionGenerator<Action>* transition_generator,
        engine_interfaces::CostFunction<State, Action>* cost_function)
        : state_id_map_(state_id_map)
        , action_id_map_(action_id_map)
        , transition_generator_(transition_generator)
        , cost_function_(cost_function)
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
     * @brief Output the list of applicable operators in the state with id
     * \p sid via \p ops.
     */
    void
    generate_applicable_ops(const StateID& sid, std::vector<Action>& ops) const
    {
        transition_generator_->generate_applicable_actions(sid, ops);
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
        transition_generator_->generate_action_transitions(sid, a, successors);
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
        transition_generator_->generate_all_transitions(sid, aops, successors);
    }

    /**
     * @brief Get the termination info for state \p s .
     */
    TerminationInfo get_termination_info(const State& s) const
    {
        return cost_function_->get_termination_info(s);
    }

    /**
     * @brief Get the action cost for action \p a when applied in the state
     * with id \p sid .
     */
    value_t get_action_cost(const StateID& sid, const Action& a) const
    {
        return cost_function_->get_action_cost(sid, a);
    }

    /**
     * @brief Get the state id map object.
     */
    engine_interfaces::StateIDMap<State>* get_state_id_map() const
    {
        return state_id_map_;
    }

    /**
     * @brief Get the action id map object.
     */
    engine_interfaces::ActionIDMap<Action>* get_action_id_map() const
    {
        return action_id_map_;
    }

    /**
     * @brief Get the transition generator.
     */
    engine_interfaces::TransitionGenerator<Action>*
    get_transition_generator() const
    {
        return transition_generator_;
    }

    /**
     * @brief Get the cost function.
     */
    engine_interfaces::CostFunction<State, Action>* get_cost_function() const
    {
        return cost_function_;
    }

private:
    engine_interfaces::StateIDMap<State>* state_id_map_;
    engine_interfaces::ActionIDMap<Action>* action_id_map_;
    engine_interfaces::TransitionGenerator<Action>* transition_generator_;
    engine_interfaces::CostFunction<State, Action>* cost_function_;
};

} // namespace engines
} // namespace probfd

#endif // __ENGINE_H__