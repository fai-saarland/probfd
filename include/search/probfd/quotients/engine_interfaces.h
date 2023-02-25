#ifndef PROBFD_QUOTIENTS_ENGINE_INTERFACES_H
#define PROBFD_QUOTIENTS_ENGINE_INTERFACES_H

#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/state_space.h"

#include "probfd/quotients/quotient_system.h"

namespace probfd {
namespace quotients {

template <typename State, typename Action>
using QuotientCostFunction =
    engine_interfaces::CostFunction<State, quotients::QuotientAction<Action>>;

template <typename State, typename Action>
class DefaultQuotientCostFunction : public QuotientCostFunction<State, Action> {
public:
    explicit DefaultQuotientCostFunction(
        quotients::QuotientSystem<State, Action>* quotient,
        engine_interfaces::CostFunction<State, Action>* orig)
        : quotient_(quotient)
        , eval_(orig)
    {
    }

    virtual TerminationInfo get_termination_info(const State& s) override
    {
        return eval_->get_termination_info(s);
    }

    virtual value_t
    get_action_cost(StateID s, quotients::QuotientAction<Action> qa) override
    {
        return eval_->get_action_cost(
            qa.state_id,
            quotient_->get_original_action(s, qa));
    }

    engine_interfaces::CostFunction<State, Action>* real() const
    {
        return eval_;
    }

private:
    quotients::QuotientSystem<State, Action>* quotient_;
    engine_interfaces::CostFunction<State, Action>* eval_;
};

} // namespace quotients

namespace engine_interfaces {
template <typename State, typename Action>
class StateSpace<State, quotients::QuotientAction<Action>> {
    quotients::QuotientSystem<State, Action>* quotient_;

public:
    explicit StateSpace(quotients::QuotientSystem<State, Action>* quotient)
        : quotient_(quotient)
    {
    }

    StateID get_state_id(const State& state)
    {
        return quotient_->get_state_id(state);
    }

    State get_state(StateID state_id)
    {
        return quotient_->get_state(state_id);
    }

    ActionID
    get_action_id(StateID sid, const quotients::QuotientAction<Action>& qa)
        const
    {
        return quotient_->get_action_id(sid, qa);
    }

    quotients::QuotientAction<Action>
    get_action(StateID state_id, ActionID action_id) const
    {
        return quotient_->get_action(state_id, action_id);
    }

    void generate_applicable_actions(
        StateID sid,
        std::vector<quotients::QuotientAction<Action>>& res) const
    {
        return quotient_->generate_applicable_ops(sid, res);
    }

    void generate_action_transitions(
        StateID sid,
        const quotients::QuotientAction<Action>& a,
        Distribution<StateID>& res) const
    {
        quotient_->generate_successors(sid, a, res);
    }

    void generate_all_transitions(
        StateID sid,
        std::vector<quotients::QuotientAction<Action>>& aops,
        std::vector<Distribution<StateID>>& succs) const
    {
        quotient_->generate_all_successors(sid, aops, succs);
    }
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __ENGINE_INTERFACES_H__