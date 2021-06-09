#pragma once

#include "../engine_interfaces/applicable_actions_generator.h"
#include "../engine_interfaces/state_evaluator.h"
#include "../engine_interfaces/state_reward_function.h"
#include "../engine_interfaces/state_id_map.h"
#include "../engine_interfaces/transition_generator.h"
#include "../storage/per_state_storage.h"

#include <deque>
#include <iostream>
#include <vector>

namespace probabilistic {
namespace graphviz {

template<
    typename State,
    typename Action,
    typename StateToString,
    typename ActionToString,
    typename GoalCheck = StateRewardFunction<State>,
    typename PruneState = StateEvaluator<State>,
    typename AOpsGen = ApplicableActionsGenerator<Action>,
    typename TGen = TransitionGenerator<Action>>
class GraphVisualization {
public:
    GraphVisualization(
        std::ostream& out,
        StateIDMap<State>* id_map,
        GoalCheck* is_goal,
        AOpsGen* aops_gen,
        TGen* transition_gen,
        StateToString* state_to_string,
        ActionToString* op_to_string,
        bool expand_terminal,
        PruneState* prune = nullptr)
        : out(out)
        , id_map_(id_map)
        , aops_gen_(aops_gen)
        , transition_gen_(transition_gen)
        , is_goal_(is_goal)
        , state_to_string_(state_to_string)
        , op_to_string_(op_to_string)
        , expand_terminals_(expand_terminal)
        , prune_(prune)
    {
    }

    void operator()(const State& initial_state)
    {
        StateID istateid = id_map_->get_state_id(initial_state);
        std::vector<Action> aops;
        std::deque<std::pair<StateID, State>> open;
        open.emplace_back(istateid, initial_state);
        storage::PerStateStorage<bool> drawn(false);
        drawn[istateid] = true;

        out << "digraph {"
            << "\n";
        out << "initial [label=\"\", shape=plaintext];"
            << "\n";
        out << "initial -> state" << istateid << ";"
            << "\n";
        unsigned transitions = 0;
        while (!open.empty()) {
            auto s = open.front();
            open.pop_front();
            auto rew = is_goal_->operator()(s.second);
            const bool print_transitions = !rew;
            bool expand = (expand_terminals_ || print_transitions);
            out << "state" << s.first << " [label=\"";
            if (state_to_string_ == nullptr) {
                out << "s" << s.first;
            } else {
                out << state_to_string_->operator()(s.second);
            }
            out << "\", shape=ellipse";
            if (rew) {
                out << ", peripheries=2";
            } else if (
                expand
                && (prune_ != nullptr && (bool)prune_->operator()(s.second))) {
                expand = false;
                out << ", peripheries=3";
            }
            out << "];"
                << "\n";
            if (expand) {
                aops_gen_->operator()(s.first, aops);
                for (unsigned i = 0; i < aops.size(); i++) {
                    Distribution<StateID> successors;
                    transition_gen_->operator()(s.first, aops[i], successors);
                    if (print_transitions) {
                        out << "transition" << transitions << " [label=\"";
                        if (op_to_string_ != nullptr) {
                            out << op_to_string_->operator()(aops[i]);
                        }
                        out << "\", shape=box];"
                            << "\n";
                        out << "state" << s.first << " -> "
                            << "transition" << transitions << "\n";
                    }
                    for (auto it = successors.begin(); it != successors.end();
                         it++) {
                        StateID succid = it->first;
                        if (!drawn[succid]) {
                            drawn[succid] = true;
                            open.emplace_back(
                                succid, id_map_->get_state(it->first));
                        }
                        if (print_transitions) {
                            out << "transition" << transitions << " -> state"
                                << succid << ";"
                                << "\n";
                        }
                    }
                    transitions++;
                }
                aops.clear();
            }
        }
        out << "}" << std::endl;
    }

private:
    std::ostream& out;
    StateIDMap<State>* id_map_;
    AOpsGen* aops_gen_;
    TGen* transition_gen_;
    GoalCheck* is_goal_;
    StateToString* state_to_string_;
    ActionToString* op_to_string_;
    const bool expand_terminals_;
    PruneState* prune_;
};

template<
    typename State,
    typename Action,
    typename StateToString,
    typename ActionToString>
void
dump(
    std::ostream& out,
    const State& state,
    StateIDMap<State>* state_id_map,
    StateRewardFunction<State>* goal_check,
    ApplicableActionsGenerator<Action>* aops_gen,
    TransitionGenerator<Action>* tgen,
    StateToString* sstr,
    ActionToString* astr,
    bool expand_terminal = false)
{
    GraphVisualization<State, Action, StateToString, ActionToString> gv(
        out,
        state_id_map,
        goal_check,
        aops_gen,
        tgen,
        sstr,
        astr,
        expand_terminal,
        nullptr);
    gv(state);
}

} // namespace graphviz
} // namespace probabilistic
