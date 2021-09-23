#ifndef MDPS_QUOTIENT_SYSTEM_HEURISTIC_SEARCH_INTERFACE_H
#define MDPS_QUOTIENT_SYSTEM_HEURISTIC_SEARCH_INTERFACE_H

#include "../engine_interfaces/dead_end_listener.h"
#include "../engine_interfaces/open_list.h"
#include "../engine_interfaces/policy_picker.h"
#include "../engine_interfaces/transition_sampler.h"
#include "quotient_system.h"

namespace probabilistic {
namespace quotient_system {

template <typename State, typename Action, typename Passthrough>
struct DeadEndListener {
    using is_default_implementation = std::true_type;

    bool operator()(const StateID&) { return false; }

    bool operator()(
        std::deque<StateID>::const_iterator,
        std::deque<StateID>::const_iterator)
    {
        return false;
    }
};

template <typename Action, typename Passthrough>
struct OpenList : public probabilistic::OpenList<Action> {
    using probabilistic::OpenList<Action>::OpenList;

    void push(
        const StateID&,
        const QuotientAction<Action>&,
        const value_type::value_t&,
        const StateID& state_id)
    {
        this->push(state_id);
    }
};

template <typename Action, typename Passthrough>
struct PolicyPicker {
    using is_default_implementation = std::true_type;

    int operator()(
        const StateID&,
        const ActionID&,
        const std::vector<QuotientAction<Action>>&,
        const std::vector<Distribution<StateID>>&)
    {
        return 0;
    }
};

template <typename Action, typename Passthrough>
struct TransitionSampler {
    using is_default_implementation = std::true_type;

    StateID operator()(
        const StateID&,
        const QuotientAction<Action>&,
        const Distribution<StateID>& transition)
    {
        return distribution_random_sampler::DistributionRandomSampler()(
            transition);
    }
};

template <typename State, typename Action>
class DeadEndListener<State, Action, std::false_type> {
public:
    explicit DeadEndListener(
        QuotientSystem<Action>* quotient,
        probabilistic::DeadEndListener<State, Action>* original)
        : quotient_(quotient)
        , original_(original)
    {
    }

    bool operator()(const StateID& state)
    {
        component_.clear();
        auto it_pair = quotient_->quotient_iterator(state);
        for (; it_pair.first != it_pair.second; ++it_pair.first) {
            component_.push_back(*it_pair.first);
        }
        return original_->operator()(component_.begin(), component_.end());
    }

    bool operator()(
        std::deque<StateID>::const_iterator begin,
        std::deque<StateID>::const_iterator end)
    {
        component_.clear();
        for (; begin != end; ++begin) {
            auto it_pair = quotient_->quotient_iterator(*begin);
            for (; it_pair.first != it_pair.second; ++it_pair.first) {
                component_.push_back(*it_pair.first);
            }
        }
        return original_->operator()(component_.begin(), component_.end());
    }

    probabilistic::DeadEndListener<State, Action>* real() const
    {
        return original_;
    }

    QuotientSystem<Action>* quotient_;
    probabilistic::DeadEndListener<State, Action>* original_;
    std::deque<StateID> component_;
};

template <typename Action>
class PolicyPicker<Action, std::false_type> {
public:
    explicit PolicyPicker(
        QuotientSystem<Action>* quotient,
        probabilistic::PolicyPicker<Action>* original)
        : quotient_(quotient)
        , original_(original)
    {
    }

    int operator()(
        const StateID& state,
        const ActionID& prev_policy,
        const std::vector<QuotientAction<Action>>& action_choices,
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

    probabilistic::PolicyPicker<Action>* real() const { return original_; }

    std::vector<Action> choices_;
    QuotientSystem<Action>* quotient_;
    probabilistic::PolicyPicker<Action>* original_;
};

template <typename Action>
class TransitionSampler<Action, std::false_type> {
public:
    explicit TransitionSampler(
        QuotientSystem<Action>* quotient,
        probabilistic::TransitionSampler<Action>* original)
        : quotient_(quotient)
        , original_(original)
    {
    }

    StateID operator()(
        const StateID& state,
        const QuotientAction<Action>& action,
        const Distribution<StateID>& transition)
    {
        const Action oa = quotient_->get_original_action(state, action);
        return original_->operator()(state, oa, transition);
    }

    probabilistic::TransitionSampler<Action>* real() const { return original_; }

    QuotientSystem<Action>* quotient_;
    probabilistic::TransitionSampler<Action>* original_;
};

template <typename Action>
class OpenList<Action, std::false_type> {
public:
    explicit OpenList(
        QuotientSystem<Action>* quotient,
        probabilistic::OpenList<Action>* original)
        : quotient_(quotient)
        , original_(original)
    {
    }

    StateID pop() { return original_->pop(); }

    void push(const StateID& state_id) { original_->push(state_id); }

    void push(
        const StateID& parent,
        const QuotientAction<Action>& action,
        const value_type::value_t& prob,
        const StateID& state_id)
    {
        const Action oa = this->quotient_->get_original_action(parent, action);
        original_->push(parent, oa, prob, state_id);
    }

    unsigned size() const { return original_->size(); }

    bool empty() const { return original_->empty(); }

    void clear() { original_->clear(); }

    probabilistic::OpenList<Action>* real() const { return original_; }

    QuotientSystem<Action>* quotient_;
    probabilistic::OpenList<Action>* original_;
};

} // namespace quotient_system

template <typename State, typename Action>
using DeadEndListenerBase = quotient_system::DeadEndListener<
    State,
    Action,
    typename is_default_implementation<DeadEndListener<State, Action>>::type>;

template <typename State, typename Action>
struct DeadEndListener<State, quotient_system::QuotientAction<Action>>
    : public DeadEndListenerBase<State, Action> {
    using Base = DeadEndListenerBase<State, Action>;
    using Base::Base;
};

template <typename Action>
using PolicyPickerBase = quotient_system::PolicyPicker<
    Action,
    typename is_default_implementation<
        probabilistic::PolicyPicker<Action>>::type>;

template <typename Action>
struct PolicyPicker<quotient_system::QuotientAction<Action>>
    : public PolicyPickerBase<Action> {        
    using Base = PolicyPickerBase<Action>;
    using Base::Base;
};

template <typename Action>
using TransitionSamplerBase = quotient_system::TransitionSampler<
    Action,
    typename is_default_implementation<TransitionSampler<Action>>::type>;

template <typename Action>
struct TransitionSampler<quotient_system::QuotientAction<Action>>
    : public TransitionSamplerBase<Action> {
    using Base = TransitionSamplerBase<Action>;
    using Base::Base;
};

template <typename Action>
using OpenListBase = quotient_system::OpenList<
    Action,
    typename is_default_implementation<OpenList<Action>>::type>;

template <typename Action>
struct OpenList<quotient_system::QuotientAction<Action>>
    : public OpenListBase<Action> {
    using Base = OpenListBase<Action>;
    using Base::Base;
};

} // namespace probabilistic

#endif // __HEURISTIC_SEARCH_INTERFACE_H__