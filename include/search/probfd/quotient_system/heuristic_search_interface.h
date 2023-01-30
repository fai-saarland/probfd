#ifndef MDPS_QUOTIENT_SYSTEM_HEURISTIC_SEARCH_INTERFACE_H
#define MDPS_QUOTIENT_SYSTEM_HEURISTIC_SEARCH_INTERFACE_H

#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engine_interfaces/policy_picker.h"
#include "probfd/engine_interfaces/transition_sampler.h"

#include "probfd/quotient_system.h"

namespace probfd {

namespace engine_interfaces {
template <>
class PolicyPicker<quotient_system::QuotientAction<OperatorID>> {
    using QuotientSystem = quotient_system::QuotientSystem<OperatorID>;
    using QuotientAction = quotient_system::QuotientAction<OperatorID>;

public:
    explicit PolicyPicker(
        QuotientSystem* quotient,
        std::shared_ptr<PolicyPicker<OperatorID>> original)
        : quotient_(quotient)
        , original_(original)
    {
    }

    int operator()(
        const StateID& state,
        const ActionID& prev_policy,
        const std::vector<QuotientAction>& action_choices,
        const std::vector<Distribution<StateID>>& successors)
    {
        const ActionID oprev =
            (prev_policy == ActionID::undefined
                 ? ActionID(ActionID::undefined)
                 : quotient_->get_original_action_id(state, prev_policy));
        choices_.clear();
        choices_.reserve(action_choices.size());
        for (unsigned i = 0; i < action_choices.size(); ++i) {
            choices_.push_back(
                quotient_->get_original_action(state, action_choices[i]));
        }
        return original_->operator()(state, oprev, choices_, successors);
    }

    std::shared_ptr<PolicyPicker<OperatorID>> real() const { return original_; }

    std::vector<OperatorID> choices_;
    QuotientSystem* quotient_;
    std::shared_ptr<PolicyPicker<OperatorID>> original_;
};

template <>
class TransitionSampler<quotient_system::QuotientAction<OperatorID>> {
    using QuotientSystem = quotient_system::QuotientSystem<OperatorID>;
    using QuotientAction = quotient_system::QuotientAction<OperatorID>;

public:
    explicit TransitionSampler(
        QuotientSystem* quotient,
        std::shared_ptr<TransitionSampler<OperatorID>> original)
        : quotient_(quotient)
        , original_(original)
    {
    }

    StateID operator()(
        const StateID& state,
        const QuotientAction& action,
        const Distribution<StateID>& transition)
    {
        const OperatorID op_id = quotient_->get_original_action(state, action);
        return original_->operator()(state, op_id, transition);
    }

    std::shared_ptr<TransitionSampler<OperatorID>> real() const
    {
        return original_;
    }

    QuotientSystem* quotient_;
    std::shared_ptr<TransitionSampler<OperatorID>> original_;
};

template <>
class OpenList<quotient_system::QuotientAction<OperatorID>> {
    using QuotientSystem = quotient_system::QuotientSystem<OperatorID>;
    using QuotientAction = quotient_system::QuotientAction<OperatorID>;

public:
    explicit OpenList(
        QuotientSystem* quotient,
        std::shared_ptr<OpenList<OperatorID>> original)
        : quotient_(quotient)
        , original_(original)
    {
    }

    StateID pop() { return original_->pop(); }

    void push(const StateID& state_id) { original_->push(state_id); }

    void push(
        const StateID& parent,
        const QuotientAction& action,
        const value_type::value_t& prob,
        const StateID& state_id)
    {
        const OperatorID op_id =
            this->quotient_->get_original_action(parent, action);
        original_->push(parent, op_id, prob, state_id);
    }

    unsigned size() const { return original_->size(); }

    bool empty() const { return original_->empty(); }

    void clear() { original_->clear(); }

    std::shared_ptr<OpenList<OperatorID>> real() const { return original_; }

    QuotientSystem* quotient_;
    std::shared_ptr<OpenList<OperatorID>> original_;
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __HEURISTIC_SEARCH_INTERFACE_H__