#pragma once

#include "../engine_interfaces/action_evaluator.h"
#include "../engine_interfaces/action_id_map.h"
#include "../engine_interfaces/applicable_actions_generator.h"
#include "../engine_interfaces/transition_generator.h"
#include "quotient_system.h"

namespace probabilistic {

template <typename Action>
using QuotientActionEvaluator =
    ActionEvaluator<quotient_system::QuotientAction<Action>>;

template <typename Action>
class QuotientActionRewardEvaluator
    : public QuotientActionEvaluator<Action>
{
public:
    explicit QuotientActionRewardEvaluator(
        quotient_system::QuotientSystem<Action>* quotient,
        ActionEvaluator<Action>* orig)
        : quotient_(quotient)
        , eval_(orig)
    {
    }

    virtual
    value_type::value_t
    evaluate(StateID s, quotient_system::QuotientAction<Action> qa) override
    {
        return eval_->operator()(
            qa.state_id, quotient_->get_original_action(s, qa));
    }

    ActionEvaluator<Action>* real() const { return eval_; }

private:
    quotient_system::QuotientSystem<Action>* quotient_;
    ActionEvaluator<Action>* eval_;
};

template<typename Action>
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

template<typename Action>
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

template<typename Action>
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
