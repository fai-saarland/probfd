#ifndef PROBFD_ENGINES_ENGINE_H
#define PROBFD_ENGINES_ENGINE_H

#include "probfd/engine_interfaces/evaluator.h"
#include "probfd/engine_interfaces/mdp.h"

#include "probfd/policies/empty_policy.h"

#include "probfd/distribution.h"
#include "probfd/interval.h"

#include <limits>
#include <memory>
#include <optional>
#include <vector>

class State;
class OperatorID;

namespace probfd {

/// This namespace contains implementations of SSP search algorithms.
namespace engines {

/**
 * @brief Interface for MDP engine implementations.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename State, typename Action>
class MDPEngineInterface {
public:
    virtual ~MDPEngineInterface() = default;

    /**
     * @brief Computes a partial policy for the input state.
     *
     * The default implementation of this method returns an empty policy.
     */
    virtual std::unique_ptr<PartialPolicy<State, Action>> compute_policy(
        param_type<State>,
        double = std::numeric_limits<double>::infinity())
    {
        return std::make_unique<policies::EmptyPolicy<State, Action>>();
    }

    /**
     * @brief Runs the MDP algorithm for the initial state \p state with a
     * maximum time limit.
     */
    virtual Interval solve(
        param_type<State> state,
        double max_time = std::numeric_limits<double>::infinity()) = 0;

    /**
     * @brief Prints algorithm statistics to the specified output stream.
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
class MDPEngine : public MDPEngineInterface<State, Action> {
    engine_interfaces::MDP<State, Action>* mdp;

public:
    /**
     * @brief Construct the MDP engine from the given MDP interface.
     */
    explicit MDPEngine(engine_interfaces::MDP<State, Action>* mdp)
        : mdp(mdp)
    {
    }

    /**
     * @brief Looks up the state id of state \p s in the state id mapping.
     */
    StateID get_state_id(param_type<State> s) const
    {
        return mdp->get_state_id(s);
    }

    /**
     * @brief Looks up the state corresponding to id \p sid in the state id
     * mapping.
     */
    State lookup_state(StateID sid) const { return mdp->get_state(sid); }

    /**
     * @brief Output the list of applicable operators in the state with id
     * \p sid via \p ops.
     */
    void
    generate_applicable_actions(StateID sid, std::vector<Action>& ops) const
    {
        mdp->generate_applicable_actions(sid, ops);
    }

    /**
     * @brief Output the successor distribution for the state with id \p sid
     * and applicable action \p a in \p successors .
     */
    void generate_action_transitions(
        StateID sid,
        param_type<Action> a,
        Distribution<StateID>& successors) const
    {
        mdp->generate_action_transitions(sid, a, successors);
    }

    /**
     * @brief Output all applicable actions and successor distributions.
     *
     * @param[in] sid - The source state.
     * @param[out] aops - The list of applicable actions.
     * @param[out] successors - The ordered list of successor distributions for
     * all applicable actions.
     */
    void generate_all_transitions(
        StateID sid,
        std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors) const
    {
        mdp->generate_all_transitions(sid, aops, successors);
    }

    /**
     * @brief Get the termination info for state \p s .
     */
    TerminationInfo get_termination_info(param_type<State> s) const
    {
        return mdp->get_termination_info(s);
    }

    /**
     * @brief Get the action cost for action \p a when applied in the state
     * with id \p sid .
     */
    value_t get_action_cost(param_type<Action> a) const
    {
        return mdp->get_action_cost(a);
    }

    /**
     * @brief Get the state space interface.
     */
    engine_interfaces::MDP<State, Action>* get_mdp() const { return mdp; }
};

} // namespace engines

using TaskMDPEngineInterface = engines::MDPEngineInterface<State, OperatorID>;

} // namespace probfd

#endif // __ENGINE_H__