#ifndef MDPS_UTILS_GRAPH_VISUALIZATION_H
#define MDPS_UTILS_GRAPH_VISUALIZATION_H

#include "../engine_interfaces/reward_function.h"
#include "../engine_interfaces/state_evaluator.h"
#include "../engine_interfaces/state_id_map.h"
#include "../engine_interfaces/transition_generator.h"
#include "../storage/per_state_storage.h"


#include <deque>
#include <iostream>
#include <vector>

namespace probabilistic {
namespace graphviz {

struct DefaultSTS {
    template <typename State>
    std::string operator()(const StateID& id, const State&)
    {
        return std::to_string(id);
    }
};

struct DefaultATS {
    template <typename Action>
    std::string operator()(const Action&)
    {
        return "";
    }
};

template <
    typename State,
    typename Action,
    typename StateToString,
    typename ActionToString>
void dump(
    std::ostream& out,
    const State& initial_state,
    StateIDMap<State>* state_id_map,
    RewardFunction<State, Action>* goal_check,
    TransitionGenerator<Action>* transition_gen,
    const StateToString& sstr = DefaultSTS(),
    const ActionToString& astr = DefaultATS(),
    StateEvaluator<State>* prune = nullptr,
    bool expand_terminal = false)
{
    StateID istateid = state_id_map->get_state_id(initial_state);
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

        auto rew = goal_check->operator()(s.second);
        const bool print_transitions = !rew;
        bool expand = (expand_terminal || print_transitions);

        out << "state" << s.first << " [label=\"" << sstr(s.first, s.second)
            << "\", shape=ellipse";

        if (rew) {
            out << ", peripheries=2";
        } else if (
            expand && (prune != nullptr && (bool)prune->operator()(s.second))) {
            expand = false;
            out << ", peripheries=3";
        }

        out << "];"
            << "\n";

        if (expand) {
            transition_gen->operator()(s.first, aops);

            for (const Action& act : aops) {
                Distribution<StateID> successors;
                transition_gen->operator()(s.first, act, successors);

                if (print_transitions) {
                    out << "transition" << transitions << " [label=\""
                        << astr(act) << "\", shape=box];"
                        << "\n"
                        << "state" << s.first << " -> "
                        << "transition" << transitions << "\n";
                }

                for (const StateID succid : successors.elements()) {
                    if (!drawn[succid]) {
                        drawn[succid] = true;
                        open.emplace_back(
                            succid,
                            state_id_map->get_state(succid));
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

} // namespace graphviz
} // namespace probabilistic

#endif // __GRAPH_VISUALIZATION_H__