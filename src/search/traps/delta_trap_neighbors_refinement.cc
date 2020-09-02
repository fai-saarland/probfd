#include "delta_trap_neighbors_refinement.h"

#include "../global_state.h"
#include "../globals.h"
#include "../heuristic.h"
#include "../option_parser.h"
#include "../partial_state.h"
#include "../plugin.h"
#include "../strips.h"

#include <algorithm>
#include <cassert>

namespace traps {

DeltaTrapHeuristicRefinement::DeltaTrapHeuristicRefinement(
    const options::Options& opts)
    : task_(strips_utils::STRIPS::get_task())
    , trap_(std::dynamic_pointer_cast<DeltaTrapHeuristic>(
          opts.get<std::shared_ptr<Heuristic>>("trap")))
    , deltas_(opts.get_list<std::shared_ptr<Heuristic>>("deltas"))
    , actions_with_fact_context_(task_->num_facts())
#if TRAP_FORMULA_AS_UB_TREE
    , component_()
#else
    , component_(task_->num_facts())
#endif
{
    // variable_order_.reserve(g_variable_domain.size());
    // for (unsigned var = 0; var < g_variable_domain.size(); var++) {
    //     variable_order_.push_back(var);
    // }
    in_goal_.resize(g_variable_domain.size(), false);
    goal_facts_.resize(g_variable_domain.size());
    for (const auto& g : g_goal) {
        in_goal_[g.first] = true;
        goal_facts_[g.first] = task_->get_fact(g.first, g.second);
    }

    strips_utils::FactSet facts;
    for (unsigned a = 0; a < task_->num_actions(); ++a) {
        const auto& action = task_->get_actions()[a];
        facts.clear();
        std::set_union(
            action.add.begin(),
            action.add.end(),
            action.del.begin(),
            action.del.end(),
            std::back_inserter(facts));
        std::inplace_merge(
            facts.begin(), facts.begin() + action.add.size(), facts.end());
        std::set_difference(
            action.pre.begin(),
            action.pre.end(),
            action.del.begin(),
            action.del.end(),
            std::back_inserter(facts));
        std::inplace_merge(
            facts.begin(),
            facts.begin() + action.add.size() + action.del.size(),
            facts.end());
        unsigned i = 0;
        for (auto pit = task_->fact_begin(); pit != task_->fact_end(); ++pit) {
            if (i < facts.size() && facts[i] == (*pit)) {
                i++;
            } else {
                actions_with_fact_context_[*pit].push_back(a);
            }
        }
    }
}

void
DeltaTrapHeuristicRefinement::add_options_to_parser(
    options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<Heuristic>>("trap");
    parser.add_list_option<std::shared_ptr<Heuristic>>("deltas", "", "[]");
    parser.add_option<bool>("caching", "", "false");
}

bool
DeltaTrapHeuristicRefinement::requires_recognized_neighbors_implicitly() const
{
    return true;
}

void
DeltaTrapHeuristicRefinement::statistics() const
{
    trap_->print_statistics();
}

struct ActionInfo {
    ActionInfo()
        : mutexes(0)
        , deletes(0)
        , transition_idx(-1)
    {
    }

    int mutexes;
    int deletes;
    int transition_idx;
};

struct ComponentElement {
    ComponentElement(ComponentElement&&) = default;
    explicit ComponentElement(
        strips_utils::FactSet&& fact_set___,
        const std::shared_ptr<strips_utils::STRIPS> task)
        : fact_set(std::move(fact_set___))
        , num_goal_mutex(0)
        , is_goal_mutex(fact_set.size(), false)
        , action_infos(task->num_actions())
    {
        {
            const strips_utils::FactSet& goal = task->get_goal_facts();
            unsigned i = 0;
            for (const auto& p : fact_set) {
                if (task->are_mutex(p, goal)) {
                    is_goal_mutex[i] = true;
                    num_goal_mutex++;
                }
                ++i;
            }
        }

        for (const auto& p : fact_set) {
            const std::vector<unsigned>& mutex_actions =
                task->get_actions_with_mutex_progression(p);
            for (int i = mutex_actions.size() - 1; i >= 0; --i) {
                action_infos[mutex_actions[i]].mutexes++;
            }
        }
        for (const auto& p : fact_set) {
            const std::vector<unsigned>& deleters =
                task->get_actions_that_delete(p);
            for (int i = deleters.size() - 1; i >= 0; --i) {
                ActionInfo& info = action_infos[deleters[i]];
                if (++info.deletes == 1 && info.mutexes == 0) {
                    info.transition_idx = transitions.size();
                    transitions.push_back(
                        task->get_actions()[deleters[i]].progress(fact_set));
                }
            }
        }
        num_transitions_before = transitions.size();
    }

    bool remains_goal_mutex(int var) const
    {
        return num_goal_mutex > 1 || !is_goal_mutex[var];
    }

    ComponentElement& operator=(ComponentElement&& element) = default;

    void
    pop(int var,
        const std::vector<unsigned>& deleters,
        const std::vector<unsigned>& in_context,
        const std::vector<unsigned>& mutexes)
    {
#if !defined(NDEBUG)
        for (unsigned i = 0; i < action_infos.size(); ++i) {
            ActionInfo& info = action_infos[i];
            assert(
                (info.transition_idx >= 0)
                == (info.deletes > 0 && info.mutexes == 0));
        }
#endif
        const strips_utils::Fact removed_fact = fact_set[var];
        fact_set.erase(fact_set.begin() + var);
        if (is_goal_mutex[var]) {
            num_goal_mutex--;
            assert(num_goal_mutex > 0);
        }
#if 0
        int di = deleters.size() - 1;
        int ci = in_context.size() - 1;
        int mi = mutexes.size() - 1;
        for (int a = action_infos.size() - 1; a >= 0; --a) {
            if (di >= 0 && static_cast<int>(deleters[di]) == a) {
                ActionInfo& info = action_infos[a];
                if (--info.deletes == 0 && info.mutexes == 0) {
                    strips_utils::FactSet().swap(transitions[info.transition_idx]);
                    info.transition_idx = -1;
                }
                di--;
            } else if (ci >= 0 && static_cast<int>(in_context[ci]) == a) {
                ActionInfo& info = action_infos[a];
                if (info.transition_idx >= 0) {
#if !defined(NDEBUG)
                    assert(strips_utils::remove(
                        removed_fact, transitions[info.transition_idx]));
#else
                    strips_utils::remove(
                        removed_fact, transitions[info.transition_idx]);
#endif
                }
                ci--;
            } else if (mi >= 0 && static_cast<int>(mutexes[mi]) == a) {
                ActionInfo& info = action_infos[a];
                assert(info.transition_idx < 0);
                if (--info.mutexes == 0 && info.deletes > 0) {
                    info.transition_idx = num_transitions_before++;
                }
                mi--;
            }
        }
#else
        for (int j = deleters.size() - 1; j >= 0; --j) {
            ActionInfo& info = action_infos[deleters[j]];
            if (--info.deletes == 0 && info.mutexes == 0) {
                strips_utils::FactSet().swap(transitions[info.transition_idx]);
                info.transition_idx = -1;
            }
            j--;
        }
        for (int j = in_context.size() - 1; j >= 0; --j) {
            ActionInfo& info = action_infos[in_context[j]];
            if (info.transition_idx >= 0) {
#if !defined(NDEBUG)
                assert(strips_utils::remove(
                    removed_fact, transitions[info.transition_idx]));
#else
                strips_utils::remove(removed_fact, transitions[info.transition_idx]);
#endif
            }
        }
        {
            int idx = num_transitions_before;
            for (int i = mutexes.size() - 1; i >= 0; --i) {
                ActionInfo& info = action_infos[mutexes[i]];
                assert(info.transition_idx < 0);
                if (--info.mutexes == 0 && info.deletes > 0) {
                    info.transition_idx = idx++;
                }
            }
            assert(idx == static_cast<int>(transitions.size()));
            num_transitions_before = idx;
        }
#endif
    }

    strips_utils::FactSet fact_set;
    int num_goal_mutex;
    std::vector<bool> is_goal_mutex;
    std::vector<ActionInfo> action_infos;
    std::vector<strips_utils::FactSet> transitions;

    int num_transitions_before;
};

state_component::StateComponentListener::Status
DeltaTrapHeuristicRefinement::on_new_component(
    state_component::StateComponent& states,
    state_component::StateComponent&)
{
    std::vector<ComponentElement> component;
    while (!states.end()) {
        const GlobalState& state = states.current();
        strips_utils::FactSet facts = task_->get_fact_set(state);
        component.emplace_back(std::move(facts), task_);
        states.next();
    }

    strips_utils::FactSet aux;
    strips_utils::FactSet aux2;
    PartialState partial_state(g_variable_domain.size());
    std::vector<unsigned> process_queue;
    bool minimized = false;
    for (int var = g_variable_domain.size() - 1; var >= 0; var--) {
        bool all_goal_mutex = true;
        for (int i = component.size() - 1; i >= 0; --i) {
            if (!component[i].remains_goal_mutex(var)) {
                all_goal_mutex = false;
                break;
            }
        }
        if (!all_goal_mutex) {
            continue;
        }

        component_.clear();
        process_queue.clear();
        for (int i = component.size() - 1; i >= 0; --i) {
            ComponentElement& element = component[i];
            aux = element.fact_set;
            aux.erase(aux.begin() + var);
#if TRAP_FORMULA_AS_UB_TREE
            if (component_.insert(aux, true)) {
#else
            if (component_.insert(aux).second) {
#endif
                process_queue.push_back(i);
            }
        }

#if !defined(NDEBUG)
        states.reset();
        while (!states.end()) {
            strips_utils::FactSet state_facts = task_->get_fact_set(states.current());
            assert(component_.contains_subset(state_facts));
            states.next();
        }
#endif

        bool trap_property = true;
        for (int j = process_queue.size() - 1; trap_property && j >= 0; --j) {
            ComponentElement& element = component[process_queue[j]];
            const strips_utils::Fact removed_fact = element.fact_set[var];
            {
                const std::vector<unsigned>& in_context =
                    actions_with_fact_context_[removed_fact];
                for (int k = in_context.size() - 1; k >= 0; --k) {
                    const ActionInfo& info =
                        element.action_infos[in_context[k]];
                    if (info.transition_idx >= 0) {
                        assert(info.deletes > 0 && info.mutexes == 0);
                        aux = element.transitions[info.transition_idx];
#if !defined(NDEBUG)
                        assert(strips_utils::remove(removed_fact, aux));
#else
                        strips_utils::remove(removed_fact, aux);
#endif
                        if (!is_covered(aux, partial_state)) {
                            trap_property = false;
                            break;
                        }
                    }
                }
            }
            if (!trap_property) {
                break;
            }
            {
                aux2 = element.fact_set;
                aux2.erase(aux2.begin() + var);
                const std::vector<unsigned>& mutex =
                    task_->get_actions_with_mutex_progression(removed_fact);
                for (int i = mutex.size() - 1; i >= 0; --i) {
                    const ActionInfo& info = element.action_infos[mutex[i]];
                    if (info.mutexes == 1 && info.deletes > 0) {
                        assert(info.transition_idx < 0);
                        aux = task_->get_actions()[mutex[i]].progress(aux2);
                        if (is_covered(aux, partial_state)) {
                            element.transitions.push_back(std::move(aux));
                            aux.clear();
                        } else {
                            trap_property = false;
                            break;
                        }
                    }
                }
            }
        }
        if (trap_property) {
            unsigned i = 0;
            for (int j = process_queue.size() - 1; j >= 0; j--) {
                assert(
                    j + 1 == static_cast<int>(process_queue.size())
                    || process_queue[j] > process_queue[j + 1]);
                if (process_queue[j] != i) {
                    component[i] = std::move(component[process_queue[j]]);
                }
                const strips_utils::Fact removed_fact = component[i].fact_set[var];
                component[i].pop(
                    var,
                    task_->get_actions_that_delete(removed_fact),
                    actions_with_fact_context_[removed_fact],
                    task_->get_actions_with_mutex_progression(removed_fact));
                i++;
            }
            component.erase(component.begin() + i, component.end());
            minimized = true;
        } else {
            for (int j = process_queue.size() - 1; j >= 0; --j) {
                ComponentElement& element = component[process_queue[j]];
                element.transitions.resize(element.num_transitions_before);
            }
        }
    }
    if (minimized) {
        assert(!component.empty());
        for (int i = component.size() - 1; i >= 0; --i) {
#if !defined(NDEBUG)
            assert(trap_->add_conjunction_to_formula(component[i].fact_set));
#else
            trap_->add_conjunction_to_formula(component[i].fact_set);
#endif
        }
#if !defined(NDEBUG)
        for (int i = component.size() - 1; i >= 0; --i) {
            assert(!component[i].fact_set.empty());
            assert(std::is_sorted(
                component[i].fact_set.begin(), component[i].fact_set.end()));
            assert(
                std::unique(
                    component[i].fact_set.begin(), component[i].fact_set.end())
                == component[i].fact_set.end());
        }
        states.reset();
        while (!states.end()) {
            strips_utils::FactSet state_facts = task_->get_fact_set(states.current());
            bool subset = false;
            for (int i = component.size() - 1; i >= 0; --i) {
                subset = subset
                    || std::includes(
                             state_facts.begin(),
                             state_facts.end(),
                             component[i].fact_set.begin(),
                             component[i].fact_set.end());
            }
            assert(subset);
            assert(trap_->is_satisfied(state_facts));
            states.next();
        }
#endif
        return Status::Mark;
    }
    return Status::KeepListening;
}

bool
DeltaTrapHeuristicRefinement::is_covered(
    const strips_utils::FactSet& fact_set,
    PartialState& partial_state)
{
    if (in_component(fact_set) || trap_->is_satisfied(fact_set)) {
        return true;
    }
    if (deltas_.empty()) {
        return false;
    }
    task_->copy_to_partial_state(fact_set, partial_state);
    return is_dead_end(partial_state);
}

bool
DeltaTrapHeuristicRefinement::in_component(const strips_utils::FactSet& fact_set)
{
    return component_.contains_subset(fact_set);
}

bool
DeltaTrapHeuristicRefinement::is_dead_end(const PartialState& partial_state)
{
    for (unsigned i = 0; i < deltas_.size(); i++) {
        deltas_[i]->evaluate(partial_state);
        if (deltas_[i]->is_dead_end()) {
            return true;
        }
    }
    return false;
}

static Plugin<state_component::StateComponentListener> _plugin(
    "trap_neighbors_refinement",
    options::parse<
        state_component::StateComponentListener,
        DeltaTrapHeuristicRefinement>);

} // namespace traps
