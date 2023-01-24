#ifndef MDPS_QUOTIENT_SYSTEM_ENGINE_INTERFACES_H
#define MDPS_QUOTIENT_SYSTEM_ENGINE_INTERFACES_H

#include "probfd/engine_interfaces/action_id_map.h"
#include "probfd/engine_interfaces/reward_function.h"
#include "probfd/engine_interfaces/transition_generator.h"

#include "probfd/quotient_system/quotient_system.h"

namespace probfd {
namespace quotient_system {

template <typename State, typename Action>
using QuotientRewardFunction = engine_interfaces::
    RewardFunction<State, quotient_system::QuotientAction<Action>>;

template <typename State, typename Action>
class DefaultQuotientRewardFunction
    : public QuotientRewardFunction<State, Action> {
public:
    explicit DefaultQuotientRewardFunction(
        quotient_system::QuotientSystem<Action>* quotient,
        engine_interfaces::RewardFunction<State, Action>* orig)
        : quotient_(quotient)
        , eval_(orig)
    {
    }

    virtual TerminationInfo get_termination_info(const State& s) override
    {
        return eval_->get_termination_info(s);
    }

    virtual value_t
    get_action_reward(StateID s, quotient_system::QuotientAction<Action> qa)
        override
    {
        return eval_->get_action_reward(
            qa.state_id,
            quotient_->get_original_action(s, qa));
    }

    engine_interfaces::RewardFunction<State, Action>* real() const
    {
        return eval_;
    }

private:
    quotient_system::QuotientSystem<Action>* quotient_;
    engine_interfaces::RewardFunction<State, Action>* eval_;
};

} // namespace quotient_system

namespace engine_interfaces {
template <typename Action>
class ActionIDMap<quotient_system::QuotientAction<Action>> {
public:
    explicit ActionIDMap(quotient_system::QuotientSystem<Action>* quotient)
        : quotient_(quotient)
    {
    }

    ActionID get_action_id(
        const StateID& sid,
        const quotient_system::QuotientAction<Action>& qa) const
    {
        return quotient_->get_action_id(sid, qa);
    }

    quotient_system::QuotientAction<Action>
    get_action(const StateID& state_id, const ActionID& action_id) const
    {
        return quotient_->get_action(state_id, action_id);
    }

private:
    quotient_system::QuotientSystem<Action>* quotient_;
};

template <typename Action>
class TransitionGenerator<quotient_system::QuotientAction<Action>> {
public:
    explicit TransitionGenerator(
        quotient_system::QuotientSystem<Action>* quotient)
        : quotient_(quotient)
    {
    }

    void generate_applicable_actions(
        const StateID& sid,
        std::vector<quotient_system::QuotientAction<Action>>& res) const
    {
        return quotient_->generate_applicable_ops(sid, res);
    }

    void generate_action_transitions(
        const StateID& sid,
        const quotient_system::QuotientAction<Action>& a,
        Distribution<StateID>& res) const
    {
        quotient_->generate_successors(sid, a, res);
    }

    void generate_all_transitions(
        const StateID& sid,
        std::vector<quotient_system::QuotientAction<Action>>& aops,
        std::vector<Distribution<StateID>>& succs) const
    {
        quotient_->generate_all_successors(sid, aops, succs);
    }

private:
    quotient_system::QuotientSystem<Action>* quotient_;
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __ENGINE_INTERFACES_H__