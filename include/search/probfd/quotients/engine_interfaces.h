#ifndef PROBFD_QUOTIENTS_ENGINE_INTERFACES_H
#define PROBFD_QUOTIENTS_ENGINE_INTERFACES_H

#include "probfd/engine_interfaces/action_id_map.h"
#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/transition_generator.h"

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
        quotients::QuotientSystem<Action>* quotient,
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
    quotients::QuotientSystem<Action>* quotient_;
    engine_interfaces::CostFunction<State, Action>* eval_;
};

} // namespace quotients

namespace engine_interfaces {
template <typename Action>
class ActionIDMap<quotients::QuotientAction<Action>> {
public:
    explicit ActionIDMap(quotients::QuotientSystem<Action>* quotient)
        : quotient_(quotient)
    {
    }

    ActionID get_action_id(
        const StateID& sid,
        const quotients::QuotientAction<Action>& qa) const
    {
        return quotient_->get_action_id(sid, qa);
    }

    quotients::QuotientAction<Action>
    get_action(const StateID& state_id, const ActionID& action_id) const
    {
        return quotient_->get_action(state_id, action_id);
    }

private:
    quotients::QuotientSystem<Action>* quotient_;
};

template <typename Action>
class TransitionGenerator<quotients::QuotientAction<Action>> {
public:
    explicit TransitionGenerator(quotients::QuotientSystem<Action>* quotient)
        : quotient_(quotient)
    {
    }

    void generate_applicable_actions(
        const StateID& sid,
        std::vector<quotients::QuotientAction<Action>>& res) const
    {
        return quotient_->generate_applicable_ops(sid, res);
    }

    void generate_action_transitions(
        const StateID& sid,
        const quotients::QuotientAction<Action>& a,
        Distribution<StateID>& res) const
    {
        quotient_->generate_successors(sid, a, res);
    }

    void generate_all_transitions(
        const StateID& sid,
        std::vector<quotients::QuotientAction<Action>>& aops,
        std::vector<Distribution<StateID>>& succs) const
    {
        quotient_->generate_all_successors(sid, aops, succs);
    }

private:
    quotients::QuotientSystem<Action>* quotient_;
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __ENGINE_INTERFACES_H__