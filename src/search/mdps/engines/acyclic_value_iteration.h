#pragma once

#include "../engine_interfaces/state_evaluator.h"
#include "../storage/per_state_storage.h"
#include "engine.h"

#include <deque>
#include <iostream>
#include <memory>

namespace probabilistic {
namespace acyclic_vi {

struct Statistics {
    unsigned long long state_expansions = 0;
    unsigned long long terminal_states = 0;
    unsigned long long goal_states = 0;
    unsigned long long pruned = 0;

    void print(std::ostream& out) const
    {
        out << "  Expanded state(s): " << state_expansions << std::endl;
        out << "  Pruned state(s): " << pruned << std::endl;
        out << "  Terminal state(s): " << terminal_states << std::endl;
        out << "  Goal state(s): " << goal_states << std::endl;
    }
};

template<typename State, typename Action>
class AcyclicValueIteration : public MDPEngine<State, Action> {
public:
    template<typename... Args>
    explicit AcyclicValueIteration(StateEvaluator<State>* prune, Args... args)
        : MDPEngine<State, Action>(args...)
        , prune_(prune)
        , expanded_states_(false)
        , values_(this->get_minimal_reward())
        , expansion_queue_()
    {
    }

    virtual void solve(const State& initial_state) override
    {
        push_state<false>(this->get_state_id(initial_state), nullptr);
        while (!expansion_queue_.empty()) {
            IncrementalExpansionInfo& e = expansion_queue_.back();
            bool c = true;
            do {
                for (; e.successor != e.transition.end(); ++e.successor) {
                    if (push_state(e.successor->first, &e)) {
                        c = false;
                        break;
                    }
                }
                if (c) {
                    if (e.t_value > e.value) {
                        e.value = e.t_value;
                    }
                    if (e.aops.empty()) {
                        values_[e.state] = e.value;
                        expansion_queue_.pop_back();
                        break;
                    } else {
                        e.t_value = e.reward
                            + this->get_action_reward(e.state, e.aops.back());
                        e.transition.clear();
                        this->generate_successors(
                            e.state, e.aops.back(), e.transition);
                        e.aops.pop_back();
                        e.successor = e.transition.begin();
                    }
                } else {
                    break;
                }
            } while (true);
        }
    }

    virtual value_type::value_t get_result(const State& initial_state) override
    {
        return values_[this->get_state_id(initial_state)];
    }

    virtual void print_statistics(std::ostream& out) const
    {
        statistics_.print(out);
    }

private:
    struct IncrementalExpansionInfo {
        IncrementalExpansionInfo(
            const StateID& state,
            value_type::value_t reward,
            value_type::value_t value,
            std::vector<Action>&& aops)
            : reward(reward)
            , value(value)
            , state(state)
            , aops(std::move(aops))
            , transition()
            , successor(transition.end())
            , t_value(value)
        {
        }
        IncrementalExpansionInfo(IncrementalExpansionInfo&& other) = default;

        const value_type::value_t reward;
        value_type::value_t value;
        const StateID state;

        std::vector<Action> aops;
        Distribution<StateID> transition;
        typename Distribution<StateID>::const_iterator successor;
        value_type::value_t t_value;
    };

    template<bool Update = true>
    bool push_state(const StateID& state_ref, IncrementalExpansionInfo* in)
    {
        if (expanded_states_[state_ref]) {
            if (Update) {
                in->t_value =
                    in->t_value + in->successor->second * values_[state_ref];
            }
            return false;
        }
        State state = this->lookup_state(state_ref);
        expanded_states_[state_ref] = true;
        if (prune_ && bool(prune_->operator()(state))) {
            ++statistics_.pruned;
            if (Update) {
                in->t_value = in->t_value
                    + in->successor->second * this->get_minimal_reward();
            }
            return false;
        }
        auto rew = this->get_state_reward(state);
        if (bool(rew)) {
            ++statistics_.terminal_states;
            ++statistics_.goal_states;
            values_[state_ref] = value_type::value_t(rew);
            if (Update) {
                in->t_value = in->t_value
                    + in->successor->second * value_type::value_t(rew);
            }
            return false;
        }
        std::vector<Action> aops;
        this->generate_applicable_ops(state_ref, aops);
        if (aops.empty()) {
            ++statistics_.terminal_states;
            if (Update) {
                in->t_value = in->t_value
                    + in->successor->second * this->get_minimal_reward();
            }
            return false;
        }
        ++statistics_.state_expansions;
        expansion_queue_.emplace_back(
            state_ref,
            value_type::value_t(rew),
            this->get_minimal_reward(),
            std::move(aops));
        return true;
    }

    StateEvaluator<State>* prune_;

    Statistics statistics_;

    storage::PerStateStorage<bool> expanded_states_;
    storage::PerStateStorage<value_type::value_t> values_;
    std::deque<IncrementalExpansionInfo> expansion_queue_;
};

} // namespace acyclic_vi
} // namespace probabilistic
