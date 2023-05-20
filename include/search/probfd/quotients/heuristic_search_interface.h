#ifndef PROBFD_QUOTIENT_SYSTEM_HEURISTIC_SEARCH_INTERFACE_H
#define PROBFD_QUOTIENT_SYSTEM_HEURISTIC_SEARCH_INTERFACE_H

#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engine_interfaces/policy_picker.h"
#include "probfd/engine_interfaces/successor_sampler.h"

#include "probfd/quotient_system.h"

namespace probfd {

namespace quotients {

template <typename Action>
class QuotientHeuristicSearchInterface
    : public engine_interfaces::HeuristicSearchInterface<Action> {
    engine_interfaces::HeuristicSearchInterface<
        quotients::QuotientAction<Action>>& wrapped;

public:
    QuotientHeuristicSearchInterface(
        engine_interfaces::HeuristicSearchInterface<
            quotients::QuotientAction<Action>>& wrapped)
        : wrapped(wrapped)
    {
    }

    virtual ~QuotientHeuristicSearchInterface() = default;

    const engines::heuristic_search::StateFlags&
    lookup_state_flags(StateID state_id) override final
    {
        return wrapped.lookup_state_flags(state_id);
    }

    value_t lookup_value(StateID state_id) override final
    {
        return wrapped.lookup_value(state_id);
    }

    Interval lookup_bounds(StateID state_id) override final
    {
        return wrapped.lookup_bounds(state_id);
    }

    std::optional<Action> lookup_policy(StateID state_id) override final
    {
        std::optional o = wrapped.lookup_policy(state_id);
        if (!o) return std::nullopt;
        return o->action;
    }
};

template <typename State, typename Action = OperatorID>
class RepresentativePolicyPicker
    : public engine_interfaces::
          PolicyPicker<State, quotients::QuotientAction<Action>> {
    using QuotientSystem = quotients::QuotientSystem<State, Action>;
    using QuotientAction = quotients::QuotientAction<Action>;

    std::vector<Action> choices_;
    QuotientSystem* quotient_;
    std::shared_ptr<engine_interfaces::PolicyPicker<State, Action>> original_;

public:
    RepresentativePolicyPicker(
        QuotientSystem* quotient,
        std::shared_ptr<engine_interfaces::PolicyPicker<State, Action>>
            original)
        : quotient_(quotient)
        , original_(original)
    {
    }

    int pick_index(
        engine_interfaces::StateSpace<State, QuotientAction>&,
        StateID state,
        std::optional<QuotientAction> prev_policy,
        const std::vector<QuotientAction>& action_choices,
        const std::vector<Distribution<StateID>>& successors,
        engine_interfaces::HeuristicSearchInterface<QuotientAction>&
            hs_interface) override
    {
        std::optional<Action> oprev = std::nullopt;

        if (prev_policy) oprev = prev_policy->action;

        choices_.clear();
        choices_.reserve(action_choices.size());
        for (unsigned i = 0; i < action_choices.size(); ++i) {
            choices_.push_back(action_choices[i].action);
        }

        QuotientHeuristicSearchInterface qhs_interface(hs_interface);
        return original_->pick_index(
            *quotient_->get_parent_state_space(),
            state,
            oprev,
            choices_,
            successors,
            qhs_interface);
    }

    void print_statistics(std::ostream& out) override
    {
        original_->print_statistics(out);
    }
};

template <typename State>
class RepresentativeSuccessorSampler
    : public engine_interfaces::SuccessorSampler<
          quotients::QuotientAction<OperatorID>> {
    using QuotientSystem = quotients::QuotientSystem<State, OperatorID>;
    using QuotientAction = quotients::QuotientAction<OperatorID>;

    QuotientSystem* quotient_;
    std::shared_ptr<engine_interfaces::SuccessorSampler<OperatorID>> original_;

public:
    RepresentativeSuccessorSampler(
        QuotientSystem* quotient,
        std::shared_ptr<engine_interfaces::SuccessorSampler<OperatorID>>
            original)
        : quotient_(quotient)
        , original_(original)
    {
    }

    StateID sample(
        StateID state,
        QuotientAction qaction,
        const Distribution<StateID>& transition,
        engine_interfaces::HeuristicSearchInterface<QuotientAction>&
            hs_interface) override
    {
        QuotientHeuristicSearchInterface qhs_interface(hs_interface);
        return original_
            ->sample(state, qaction.action, transition, qhs_interface);
    }

    void print_statistics(std::ostream& out) const override
    {
        original_->print_statistics(out);
    }
};

template <typename State>
class RepresentativeOpenList
    : public engine_interfaces::OpenList<
          quotients::QuotientAction<OperatorID>> {
    using QuotientSystem = quotients::QuotientSystem<State, OperatorID>;
    using QuotientAction = quotients::QuotientAction<OperatorID>;

    QuotientSystem* quotient_;
    std::shared_ptr<engine_interfaces::OpenList<OperatorID>> original_;

public:
    RepresentativeOpenList(
        QuotientSystem* quotient,
        std::shared_ptr<engine_interfaces::OpenList<OperatorID>> original)
        : quotient_(quotient)
        , original_(original)
    {
    }

    StateID pop() override { return original_->pop(); }

    void push(StateID state_id) override { original_->push(state_id); }

    /*void
    /push(StateID parent, QuotientAction qaction, value_t prob, StateID
    state_id) override
    {
        original_->push(parent, qaction.action, prob, state_id);
    }*/

    unsigned size() const override { return original_->size(); }

    bool empty() const override { return original_->empty(); }

    void clear() override { original_->clear(); }
};

} // namespace quotients
} // namespace probfd

#endif // __HEURISTIC_SEARCH_INTERFACE_H__