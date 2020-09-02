#pragma once

#include "../distribution.h"
#include "../interfaces/i_engine.h"
#include "../value_type.h"
#include "types_common.h"

#include <deque>
#include <unordered_set>
#include <vector>

namespace probabilistic {
namespace algorithms {

namespace heuristic_search_base {
class HeuristicSearchStateBaseInformation;
}

template<typename State, typename Action>
class HeuristicSearchStatusInterface {
public:
    virtual ~HeuristicSearchStatusInterface() = default;
    virtual const heuristic_search_base::HeuristicSearchStateBaseInformation*
    lookup_state_info(const State& state) = 0;
};

template<typename State, typename Action>
struct PolicyChooser {
    void connect(HeuristicSearchStatusInterface<State, Action>* hsinterface);

    unsigned operator()(
        const bool unseen,
        const Action* prev_policy,
        const State& state,
        const std::vector<std::pair<Action, Distribution<State>>>&
            greedy_optimal_transitions);
};

template<typename State, typename Action>
struct TransitionSampler {
    void connect(HeuristicSearchStatusInterface<State, Action>* hsinstance);

    State operator()(
        const State& state,
        const Action& action,
        const Distribution<State>& transition);
};

template<typename State, typename Action>
struct NewStateHandler {
    virtual ~NewStateHandler() = default;
    virtual void touch_goal(const State& state) = 0;
    virtual void touch_dead_end(const State& state) = 0;
    virtual void touch(const State& state) = 0;
};

template<typename State>
class StateComponentListener {
public:
    bool neighbor_dependency() const;
    bool operator()(
        const State& state,
        typename std::unordered_set<State>::const_iterator child_begin,
        typename std::unordered_set<State>::const_iterator child_end);
    bool operator()(
        typename std::deque<State>::const_iterator state_begin,
        typename std::deque<State>::const_iterator state_end,
        typename std::unordered_set<State>::const_iterator child_begin,
        typename std::unordered_set<State>::const_iterator child_end);
};

template<typename State, typename Action>
class StateListener {
public:
    using DefaultImplementation = std::true_type;

    explicit StateListener(
        StateComponentListener<State>* clist,
        ApplicableActionsGenerator<State, Action>* aops_gen,
        TransitionGenerator<State, Action>* transition_gen)
        : listener_(clist)
        , aops_gen_(aops_gen)
        , transition_gen_(transition_gen)
    {
    }

    ~StateListener() = default;

    bool operator()(const State& state)
    {
        if (listener_->neighbor_dependency()) {
            component_.insert(state);
            feed_neighbors();
        }
        const bool r = listener_->operator()(
            state, successors_.begin(), successors_.end());
        component_.clear();
        successors_.clear();
        return r;
    }

    bool operator()(
        typename std::deque<State>::const_iterator begin,
        typename std::deque<State>::const_iterator end)
    {
        if (listener_->neighbor_dependency()) {
            for (auto it = begin; it != end; ++it) {
                component_.insert(*it);
            }
            feed_neighbors();
        }
        const bool r = listener_->operator()(
            begin, end, successors_.begin(), successors_.end());
        component_.clear();
        successors_.clear();
        return r;
    }

    StateComponentListener<State>* get_state_component_listener() const
    {
        return listener_;
    }

private:
    void feed_neighbors()
    {
        for (auto it = component_.begin(); it != component_.end(); ++it) {
            aops_gen_->operator()(*it, aops_);
            for (int i = aops_.size() - 1; i >= 0; --i) {
                transition_ = transition_gen_->operator()(*it, aops_[i]);
                for (auto child = transition_.begin();
                     child != transition_.end();
                     ++child) {
                    if (!component_.count(child->first)) {
                        successors_.insert(child->first);
                    }
                }
            }
            aops_.clear();
        }
    }

    StateComponentListener<State>* listener_;
    ApplicableActionsGenerator<State, Action>* aops_gen_;
    TransitionGenerator<State, Action>* transition_gen_;
    std::vector<Action> aops_;
    Distribution<State> transition_;
    std::unordered_set<State> component_;
    std::unordered_set<State> successors_;
};

template<typename State, typename Action>
struct OpenList {
    bool empty();
    unsigned size();
    void clear();
    StateID pop();
    void push(const StateID& state_id, const State& state);
    void push(
        const StateID& state_id,
        const State& parent,
        const Action& action,
        const value_type::value_t& prob,
        const State& state);
};

template<typename Action>
struct NullAction {
    Action operator()();
};

enum class DeadEndIdentificationLevel {
    Off,
    Policy,
    Visited,
    Complete,
};

} // namespace algorithms
} // namespace probabilistic
