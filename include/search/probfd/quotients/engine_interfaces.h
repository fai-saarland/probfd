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

    virtual TerminationInfo get_termination_info(param_type<State> s) override
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
} // namespace probfd

#endif // __ENGINE_INTERFACES_H__