#pragma once

#include "../distribution.h"
#include "../engine_interfaces/action_evaluator.h"
#include "../engine_interfaces/action_id_map.h"
#include "../engine_interfaces/applicable_actions_generator.h"
#include "../engine_interfaces/state_evaluator.h"
#include "../engine_interfaces/state_id_map.h"
#include "../engine_interfaces/transition_generator.h"
#include "../value_type.h"

#include <vector>

namespace probabilistic {
namespace engines {

template<typename State>
class MDPEngineInterface {
public:
    virtual ~MDPEngineInterface() = default;
    virtual void reset_solver_state() { }
    virtual void solve(const State& state) = 0;
    virtual value_type::value_t get_result(const State& state) = 0;
    virtual bool supports_error_bound() const { return false; }
    virtual value_type::value_t get_error(const State& ) { return 0; }
    virtual void print_statistics(std::ostream&) const { }
};

template<typename State, typename Action>
class MDPEngine : public MDPEngineInterface<State> {
public:
    explicit MDPEngine(
        StateIDMap<State>* state_id_map,
        ActionIDMap<Action>* action_id_map,
        StateEvaluator<State>* state_reward_function,
        ActionEvaluator<Action>* action_reward_function,
        value_type::value_t minimal_reward,
        value_type::value_t maximal_reward,
        ApplicableActionsGenerator<Action>* aops_generator,
        TransitionGenerator<Action>* transition_generator)
        : state_id_map_(state_id_map)
        , action_id_map_(action_id_map)
        , state_reward_function_(state_reward_function)
        , action_reward_function_(action_reward_function)
        , minimal_reward_(minimal_reward)
        , maximal_reward_(maximal_reward)
        , aops_generator_(aops_generator)
        , transition_generator_(transition_generator)
    {
    }

    StateID get_state_id(const State& s) const
    {
        return state_id_map_->get_state_id(s);
    }

    State lookup_state(const StateID& sid) const
    {
        return state_id_map_->get_state(sid);
    }

    ActionID get_action_id(const StateID& sid, const Action& a) const
    {
        return action_id_map_->get_action_id(sid, a);
    }

    Action lookup_action(const StateID& sid, const ActionID& aid) const
    {
        return action_id_map_->get_action(sid, aid);
    }

    EvaluationResult get_state_reward(const State& s) const
    {
        return state_reward_function_->operator()(s);
    }

    value_type::value_t
    get_action_reward(const StateID& sid, const Action& a) const
    {
        return action_reward_function_->operator()(sid, a);
    }

    value_type::value_t get_minimal_reward() const { return minimal_reward_; }

    value_type::value_t get_maximal_reward() const { return maximal_reward_; }

    void
    generate_applicable_ops(const StateID& sid, std::vector<Action>& ops) const
    {
        aops_generator_->operator()(sid, ops);
    }

    void generate_successors(
        const StateID& sid,
        const Action& a,
        Distribution<StateID>& successors) const
    {
        transition_generator_->operator()(sid, a, successors);
    }

    void generate_all_successors(
        const StateID& sid,
        std::vector<Action>& aops,
        std::vector<Distribution<StateID>>& successors) const
    {
        transition_generator_->operator()(sid, aops, successors);
    }

    StateIDMap<State>* get_state_id_map() const { return state_id_map_; }

    ActionIDMap<Action>* get_action_id_map() const { return action_id_map_; }

    StateEvaluator<State>* get_state_reward_function() const
    {
        return state_reward_function_;
    }

    ActionEvaluator<Action>* get_action_reward_function() const
    {
        return action_reward_function_;
    }

    ApplicableActionsGenerator<Action>* get_applicable_actions_generator() const
    {
        return aops_generator_;
    }

    TransitionGenerator<Action>* get_transition_generator() const
    {
        return transition_generator_;
    }

private:
    StateIDMap<State>* state_id_map_;
    ActionIDMap<Action>* action_id_map_;
    StateEvaluator<State>* state_reward_function_;
    ActionEvaluator<Action>* action_reward_function_;
    const value_type::value_t minimal_reward_;
    const value_type::value_t maximal_reward_;
    ApplicableActionsGenerator<Action>* aops_generator_;
    TransitionGenerator<Action>* transition_generator_;
};

} // namespace engines
} // namespace probabilistic
