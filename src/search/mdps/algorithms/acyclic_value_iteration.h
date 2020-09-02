#pragma once

#include "../analysis_result.h"
#include "../interfaces/i_engine.h"
#include "../interfaces/i_printable.h"
#include "types_common.h"
#include "types_storage.h"

#include <deque>
#include <iostream>
#include <memory>

namespace probabilistic {
namespace algorithms {
namespace acyclic_vi {

struct Statistics : public IPrintable {
    unsigned long long state_expansions;
    unsigned long long state_generations;
    unsigned long long transition_generations;
    unsigned long long terminal_states;
    unsigned long long goal_states;
    unsigned long long pruned;

    Statistics()
        : state_expansions(0)
        , state_generations(0)
        , transition_generations(0)
        , terminal_states(0)
        , goal_states(0)
        , pruned(0)
    {
    }

    Statistics(const Statistics&) = default;
    Statistics(Statistics&&) = default;
    ~Statistics() = default;
    Statistics& operator=(const Statistics&) = default;
    Statistics& operator=(Statistics&&) = default;

    virtual void print(std::ostream& out) const override
    {
        out << "**** Acyclic Value Iteration ****" << std::endl;
        out << "Expanded state(s): " << state_expansions << std::endl;
        out << "Terminal state(s): " << terminal_states << std::endl;
        out << "Goal state(s): " << goal_states << std::endl;
        out << "Generated state(s): " << state_generations << std::endl;
        out << "Pruned state(s): " << pruned << std::endl;
        out << "Generated transition(s): " << transition_generations
            << std::endl;
    }
};

template<typename State, typename Action>
class AcyclicValueIteration : public IMDPEngine<State> {
public:
    AcyclicValueIteration(
        StateIDMap<State>* id_map,
        ApplicableActionsGenerator<State, Action>* tgen,
        TransitionGenerator<State, Action>* sgen,
        StateEvaluationFunction<State>* state_reward,
        TransitionRewardFunction<State, Action>* transition_reward,
        value_type::value_t dead_end_value,
        StateEvaluationFunction<State>* prune)
        : state_reward_(state_reward)
        , transition_reward_(transition_reward)
        , dead_end_value_(dead_end_value)
        , prune_(prune)
        , state_refs_(id_map)
        , transition_generator_(tgen)
        , successor_generator_(sgen)
        , expanded_states_(false)
        , values_(dead_end_value_)
        , expansion_queue_()
    {
    }

    virtual AnalysisResult solve(const State& initial_state) override
    {
        push_state<false>(initial_state, nullptr);
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
                        values_[state_refs_->operator[](e.state)] = e.value;
                        expansion_queue_.pop_back();
                        break;
                    } else {
                        e.t_value = e.reward
                            + transition_reward_->operator()(
                                e.state, e.aops.back());
                        e.transition = successor_generator_->operator()(
                            e.state, e.aops.back());
                        e.aops.pop_back();
                        e.successor = e.transition.begin();
                        statistics_.state_generations += e.transition.size();
                    }
                } else {
                    break;
                }
            } while (true);
        }
        return AnalysisResult(
            values_[state_refs_->operator[](initial_state)],
            new Statistics(statistics_));
    }

private:
    struct IncrementalExpansionInfo {
        IncrementalExpansionInfo(
            const State& state,
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
        const State state;

        std::vector<Action> aops;
        Distribution<State> transition;
        typename Distribution<State>::const_iterator successor;
        value_type::value_t t_value;
    };

    template<bool Update = true>
    bool push_state(const State& state, IncrementalExpansionInfo* in)
    {
        StateID state_ref = state_refs_->operator[](state);
        if (expanded_states_[state_ref]) {
            if (Update) {
                in->t_value =
                    in->t_value + in->successor->second * values_[state_ref];
            }
            return false;
        }
        expanded_states_[state_ref] = true;
        if (prune_ && prune_->operator()(state)) {
            ++statistics_.pruned;
            if (Update) {
                in->t_value =
                    in->t_value + in->successor->second * dead_end_value_;
            }
            return false;
        }
        auto rew = state_reward_->operator()(state);
        if ((bool)rew) {
            ++statistics_.terminal_states;
            ++statistics_.goal_states;
            values_[state_ref] = (value_type::value_t)rew;
            if (Update) {
                in->t_value = in->t_value
                    + in->successor->second * ((value_type::value_t)rew);
            }
            return false;
        }
        std::vector<Action> aops;
        transition_generator_->operator()(state, aops);
        if (aops.empty()) {
            ++statistics_.terminal_states;
            if (Update) {
                in->t_value =
                    in->t_value + in->successor->second * dead_end_value_;
            }
            return false;
        }
        ++statistics_.state_expansions;
        statistics_.transition_generations += aops.size();
        expansion_queue_.emplace_back(
            state, (value_type::value_t)rew, dead_end_value_, std::move(aops));
        return true;
    }

    StateEvaluationFunction<State>* state_reward_;
    TransitionRewardFunction<State, Action>* transition_reward_;
    const value_type::value_t dead_end_value_;
    StateEvaluationFunction<State>* prune_;

    StateIDMap<State>* state_refs_;
    ApplicableActionsGenerator<State, Action>* transition_generator_;
    TransitionGenerator<State, Action>* successor_generator_;

    Statistics statistics_;

    storage::PerStateStorage<bool> expanded_states_;
    storage::PerStateStorage<value_type::value_t> values_;
    std::deque<IncrementalExpansionInfo> expansion_queue_;
};

} // namespace acyclic_vi
} // namespace algorithms
} // namespace probabilistic
