#ifndef PROBFD_QUOTIENT_SYSTEM_HEURISTIC_SEARCH_INTERFACE_H
#define PROBFD_QUOTIENT_SYSTEM_HEURISTIC_SEARCH_INTERFACE_H

#include "probfd/engines/fdr_types.h"
#include "probfd/engines/open_list.h"
#include "probfd/engines/policy_picker.h"
#include "probfd/engines/successor_sampler.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/evaluator.h"

namespace probfd {

namespace quotients {

template <typename State, typename Action>
class QuotientMaxHeuristic : public Evaluator<QuotientState<State, Action>> {
    using QuotientState = QuotientState<State, Action>;

    const Evaluator<State>& original_;

public:
    QuotientMaxHeuristic(const Evaluator<State>& original)
        : original_(original)
    {
    }

    value_t evaluate(param_type<QuotientState> state) const override
    {
        return state.member_maximum(
            std::bind_front(&Evaluator<State>::evaluate, std::ref(original_)));
    }

    void print_statistics() const final override
    {
        original_.print_statistics();
    }
};

template <typename State, typename Action = OperatorID>
class RepresentativePolicyPicker
    : public engines::
          PolicyPicker<QuotientState<State, Action>, QuotientAction<Action>> {
    using QuotientSystem = QuotientSystem<State, Action>;
    using QuotientState = QuotientState<State, Action>;
    using QuotientAction = QuotientAction<Action>;

    std::vector<Transition<Action>> choices_;
    std::shared_ptr<engines::PolicyPicker<State, Action>> original_;

public:
    RepresentativePolicyPicker(
        std::shared_ptr<engines::PolicyPicker<State, Action>> original)
        : original_(original)
    {
    }

    int pick_index(
        MDP<QuotientState, QuotientAction>& mdp,
        StateID state,
        std::optional<QuotientAction> prev_policy,
        const std::vector<Transition<QuotientAction>>& greedy_transitions,
        engines::StateProperties& properties) override
    {
        assert(dynamic_cast<QuotientSystem*>(&mdp));
        QuotientSystem& quotient = static_cast<QuotientSystem&>(mdp);

        std::optional<Action> oprev = std::nullopt;

        if (prev_policy) oprev = prev_policy->action;

        choices_.clear();
        choices_.reserve(greedy_transitions.size());
        for (const auto& [action, dist] : greedy_transitions) {
            choices_.emplace_back(action.action, dist);
        }

        return original_->pick_index(
            quotient.get_parent_mdp(),
            state,
            oprev,
            choices_,
            properties);
    }

    void print_statistics(std::ostream& out) override
    {
        original_->print_statistics(out);
    }
};

class RepresentativeSuccessorSampler
    : public engines::SuccessorSampler<QuotientAction<OperatorID>> {
    using QuotientAction = QuotientAction<OperatorID>;

    std::shared_ptr<FDRSuccessorSampler> original_;

public:
    RepresentativeSuccessorSampler(
        std::shared_ptr<FDRSuccessorSampler> original)
        : original_(original)
    {
    }

    StateID sample(
        StateID state,
        QuotientAction qaction,
        const Distribution<StateID>& transition,
        engines::StateProperties& properties) override
    {
        return original_->sample(state, qaction.action, transition, properties);
    }

    void print_statistics(std::ostream& out) const override
    {
        original_->print_statistics(out);
    }
};

class RepresentativeOpenList
    : public engines::OpenList<QuotientAction<OperatorID>> {
    using QuotientAction = QuotientAction<OperatorID>;

    std::shared_ptr<FDROpenList> original_;

public:
    RepresentativeOpenList(std::shared_ptr<FDROpenList> original)
        : original_(original)
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

#endif // __state_properties.h_H__