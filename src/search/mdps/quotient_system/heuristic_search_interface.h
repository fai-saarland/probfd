#ifndef MDPS_QUOTIENT_SYSTEM_HEURISTIC_SEARCH_INTERFACE_H
#define MDPS_QUOTIENT_SYSTEM_HEURISTIC_SEARCH_INTERFACE_H

#include "../engine_interfaces/open_list.h"
#include "../engine_interfaces/policy_picker.h"
#include "../engine_interfaces/transition_sampler.h"

#include "../open_list.h"
#include "../policy_picker.h"
#include "../transition_sampler.h"

#include "quotient_system.h"

namespace probabilistic {

template <>
class PolicyPicker<
    quotient_system::QuotientAction<const ProbabilisticOperator*>> {
    using QuotientSystem =
        quotient_system::QuotientSystem<const ProbabilisticOperator*>;
    using QuotientAction =
        quotient_system::QuotientAction<const ProbabilisticOperator*>;

public:
    explicit PolicyPicker(
        QuotientSystem* quotient,
        PolicyPicker<const ProbabilisticOperator*>* original)
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

    PolicyPicker<const ProbabilisticOperator*>* real() const
    {
        return original_;
    }

    std::vector<const ProbabilisticOperator*> choices_;
    QuotientSystem* quotient_;
    PolicyPicker<const ProbabilisticOperator*>* original_;
};

template <>
class TransitionSampler<
    quotient_system::QuotientAction<const ProbabilisticOperator*>> {
    using QuotientSystem =
        quotient_system::QuotientSystem<const ProbabilisticOperator*>;
    using QuotientAction =
        quotient_system::QuotientAction<const ProbabilisticOperator*>;

public:
    explicit TransitionSampler(
        QuotientSystem* quotient,
        probabilistic::TransitionSampler<const ProbabilisticOperator*>*
            original)
        : quotient_(quotient)
        , original_(original)
    {
    }

    StateID operator()(
        const StateID& state,
        const QuotientAction& action,
        const Distribution<StateID>& transition)
    {
        const auto* oa = quotient_->get_original_action(state, action);
        return original_->operator()(state, oa, transition);
    }

    TransitionSampler<const ProbabilisticOperator*>* real() const
    {
        return original_;
    }

    QuotientSystem* quotient_;
    TransitionSampler<const ProbabilisticOperator*>* original_;
};

template <>
class OpenList<quotient_system::QuotientAction<const ProbabilisticOperator*>> {
    using QuotientSystem =
        quotient_system::QuotientSystem<const ProbabilisticOperator*>;
    using QuotientAction =
        quotient_system::QuotientAction<const ProbabilisticOperator*>;

public:
    explicit OpenList(
        QuotientSystem* quotient,
        OpenList<const ProbabilisticOperator*>* original)
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
        const auto* oa = this->quotient_->get_original_action(parent, action);
        original_->push(parent, oa, prob, state_id);
    }

    unsigned size() const { return original_->size(); }

    bool empty() const { return original_->empty(); }

    void clear() { original_->clear(); }

    OpenList<const ProbabilisticOperator*>* real() const { return original_; }

    QuotientSystem* quotient_;
    OpenList<const ProbabilisticOperator*>* original_;
};

} // namespace probabilistic

#endif // __HEURISTIC_SEARCH_INTERFACE_H__