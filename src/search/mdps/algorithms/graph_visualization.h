#pragma once

#include "types_common.h"
#include "types_storage.h"

#include <deque>
#include <iostream>
#include <vector>

namespace probabilistic {
namespace algorithms {
namespace graphviz {

template<
    typename State,
    typename Action,
    typename StateToString,
    typename ActionToString>
class GraphVisualization {
public:
    GraphVisualization(
        StateIDMap<State>* id_map,
        ApplicableActionsGenerator<State, Action>* aops_gen,
        TransitionGenerator<State, Action>* transition_gen,
        StateEvaluationFunction<State>* state_reward,
        StateToString* state_to_string,
        ActionToString* op_to_string,
        bool expand_terminal)
        : id_map_(id_map)
        , aops_gen_(aops_gen)
        , transition_gen_(transition_gen)
        , state_reward_(state_reward)
        , state_to_string_(state_to_string)
        , op_to_string_(op_to_string)
        , expand_terminals_(expand_terminal)
    {
    }

    void operator()(std::ostream& out, const State& initial_state)
    {
        StateID istateid = id_map_->operator[](initial_state);
        std::vector<Action> aops;
        std::deque<std::pair<StateID, State>> open;
        open.emplace_back(istateid, initial_state);
        storage::PerStateStorage<bool> drawn(false, id_map_);
        drawn[istateid] = true;

        out << "digraph {"
            << "\n";
        out << "initial [label=\"\", shape=plaintext];"
            << "\n";
        out << "initial -> state" << istateid.hash() << ";"
            << "\n";
        unsigned transitions = 0;
        while (!open.empty()) {
            auto s = open.front();
            open.pop_front();
            auto rew = state_reward_->operator()(s.second);
            out << "state" << s.first.hash() << " [label=\"";
            if (state_to_string_ == nullptr) {
                out << "s" << s.first.hash();
            } else {
                out << state_to_string_->operator()(s.second);
            }
            out << "\", shape=ellipse";
            if (rew) {
                out << ", peripheries=2";
            }
            out << "];"
                << "\n";
            const bool print_transitions = !rew;
            if (expand_terminals_ || print_transitions) {
                aops_gen_->operator()(s.second, aops);
                for (unsigned i = 0; i < aops.size(); i++) {
                    Distribution<State> successors =
                        transition_gen_->operator()(s.second, aops[i]);
                    if (print_transitions) {
                        out << "transition" << transitions << " [label=\"";
                        if (op_to_string_ != nullptr) {
                            out << op_to_string_->operator()(aops[i]);
                        }
                        out << "\", shape=box];"
                            << "\n";
                        out << "state" << s.first.hash() << " -> "
                            << "transition" << transitions << "\n";
                    }
                    for (auto it = successors.begin(); it != successors.end();
                         it++) {
                        StateID succid = this->id_map_->operator[](it->first);
                        if (!drawn[succid]) {
                            drawn[succid] = true;
                            open.emplace_back(succid, it->first);
                        }
                        if (print_transitions) {
                            out << "transition" << transitions << " -> state"
                                << succid.hash() << ";"
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
    StateIDMap<State>* id_map_;
    ApplicableActionsGenerator<State, Action>* aops_gen_;
    TransitionGenerator<State, Action>* transition_gen_;
    StateEvaluationFunction<State>* state_reward_;
    StateToString* state_to_string_;
    ActionToString* op_to_string_;
    const bool expand_terminals_;
};

} // namespace graphviz
} // namespace algorithms
} // namespace probabilistic
