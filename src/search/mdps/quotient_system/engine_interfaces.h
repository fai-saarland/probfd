#ifndef MDPS_QUOTIENT_SYSTEM_ENGINE_INTERFACES_H
#define MDPS_QUOTIENT_SYSTEM_ENGINE_INTERFACES_H

#include "../engine_interfaces/action_reward_function.h"
#include "../engine_interfaces/action_id_map.h"
#include "../engine_interfaces/applicable_actions_generator.h"
#include "../engine_interfaces/transition_generator.h"
#include "quotient_system.h"

namespace probabilistic {
namespace quotient_system {

template <typename Action>
using QuotientActionRewardFunction =
    ActionRewardFunction<quotient_system::QuotientAction<Action>>;

template <typename Action>
class DefaultQuotientActionRewardFunction
    : public QuotientActionRewardFunction<Action> {
public:
    explicit DefaultQuotientActionRewardFunction(
        quotient_system::QuotientSystem<Action>* quotient,
        ActionRewardFunction<Action>* orig)
        : quotient_(quotient)
        , eval_(orig)
    {
    }

    virtual value_type::value_t
    evaluate(StateID s, quotient_system::QuotientAction<Action> qa) override
    {
        return eval_->operator()(
            qa.state_id,
            quotient_->get_original_action(s, qa));
    }

    ActionRewardFunction<Action>* real() const { return eval_; }

private:
    quotient_system::QuotientSystem<Action>* quotient_;
    ActionRewardFunction<Action>* eval_;
};

} // namespace quotient_system

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
class ApplicableActionsGenerator<quotient_system::QuotientAction<Action>> {
public:
    explicit ApplicableActionsGenerator(
        quotient_system::QuotientSystem<Action>* quotient)
        : quotient_(quotient)
    {
    }

    void operator()(
        const StateID& sid,
        std::vector<quotient_system::QuotientAction<Action>>& res) const
    {
        return quotient_->generate_applicable_ops(sid, res);
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

    void operator()(
        const StateID& sid,
        const quotient_system::QuotientAction<Action>& a,
        Distribution<StateID>& res) const
    {
        quotient_->generate_successors(sid, a, res);
    }

    void operator()(
        const StateID& sid,
        std::vector<quotient_system::QuotientAction<Action>>& aops,
        std::vector<Distribution<StateID>>& succs) const
    {
        quotient_->generate_all_successors(sid, aops, succs);
    }

private:
    quotient_system::QuotientSystem<Action>* quotient_;
};

} // namespace probabilistic

#endif // __ENGINE_INTERFACES_H__