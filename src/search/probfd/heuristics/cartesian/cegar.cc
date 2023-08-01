#include "probfd/heuristics/cartesian/cegar.h"

#include "probfd/heuristics/cartesian/abstract_state.h"
#include "probfd/heuristics/cartesian/abstraction.h"
#include "probfd/heuristics/cartesian/flaw.h"
#include "probfd/heuristics/cartesian/flaw_generator.h"
#include "probfd/heuristics/cartesian/probabilistic_transition_system.h"
#include "probfd/heuristics/cartesian/split_selector.h"
#include "probfd/heuristics/cartesian/types.h"
#include "probfd/heuristics/cartesian/utils.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/policy.h"
#include "probfd/utils/guards.h"

#include "downward/cegar/cartesian_set.h"
#include "downward/cegar/refinement_hierarchy.h"
#include "downward/cegar/utils.h"

#include "downward/utils/math.h"
#include "downward/utils/memory.h"

#include "downward/state_registry.h"
#include "probfd/storage/per_state_storage.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_map>

using namespace std;

namespace probfd {
namespace heuristics {
namespace cartesian {

CEGAR::CEGAR(
    const shared_ptr<ProbabilisticTask>& task,
    int max_states,
    int max_non_looping_transitions,
    double max_time,
    FlawGeneratorFactory& flaw_generator_factory,
    SplitSelectorFactory& split_selector_factory,
    utils::LogProxy log)
    : task_proxy(*task)
    , domain_sizes(cegar::get_domain_sizes(task_proxy))
    , max_states(max_states)
    , max_non_looping_transitions(max_non_looping_transitions)
    , flaw_generator(flaw_generator_factory.create_flaw_generator())
    , split_selector(split_selector_factory.create_split_selector(task))
    , log(std::move(log))
    , timer(max_time)
    , refinement_hierarchy(new RefinementHierarchy(task))
    , abstraction(new Abstraction(task_proxy, log))
    , cost_function(
          *abstraction,
          task_properties::get_operator_costs(task_proxy))
{
    assert(max_states >= 1);
    if (log.is_at_least_normal()) {
        log << "Start building abstraction." << endl;
        log << "Maximum number of abstract states: " << max_states << endl;
        log << "Maximum number of abstract transitions: "
            << max_non_looping_transitions << endl;
    }

    refinement_loop();
    if (log.is_at_least_normal()) {
        log << "Done building abstraction." << endl;
        log << "Time for building abstraction: " << timer.get_elapsed_time()
            << endl;
        print_statistics();
    }
}

CEGAR::~CEGAR() = default;

std::unique_ptr<RefinementHierarchy> CEGAR::extract_refinement_hierarchy()
{
    assert(refinement_hierarchy);
    return std::move(refinement_hierarchy);
}

unique_ptr<Abstraction> CEGAR::extract_abstraction()
{
    assert(abstraction);
    return std::move(abstraction);
}

CartesianHeuristic& CEGAR::get_heuristic()
{
    return heuristic;
}

bool CEGAR::may_keep_refining() const
{
    if (abstraction->get_num_states() >= max_states) {
        if (log.is_at_least_normal()) {
            log << "Reached maximum number of states." << endl;
        }
        return false;
    } else if (
        abstraction->get_transition_system().get_num_non_loops() >=
        max_non_looping_transitions) {
        if (log.is_at_least_normal()) {
            log << "Reached maximum number of transitions." << endl;
        }
        return false;
    } else if (!utils::extra_memory_padding_is_reserved()) {
        if (log.is_at_least_normal()) {
            log << "Reached memory limit." << endl;
        }
        return false;
    }
    return true;
}

void CEGAR::separate_facts_unreachable_before_goal(utils::Timer& timer)
{
    assert(abstraction->get_goals().size() == 1);
    assert(abstraction->get_num_states() == 1);
    assert(task_proxy.get_goals().size() == 1);
    FactProxy goal = task_proxy.get_goals()[0];
    utils::HashSet<FactProxy> reachable_facts =
        get_relaxed_possible_before(task_proxy, goal);
    for (VariableProxy var : task_proxy.get_variables()) {
        if (!may_keep_refining()) break;
        int var_id = var.get_id();
        vector<int> unreachable_values;
        for (int value = 0; value < var.get_domain_size(); ++value) {
            FactProxy fact = var.get_fact(value);
            if (reachable_facts.count(fact) == 0)
                unreachable_values.push_back(value);
        }
        if (!unreachable_values.empty()) {
            TimerScope scope(timer);
            refine_abstraction(
                abstraction->get_initial_state(),
                var_id,
                unreachable_values);
        }
    }
    abstraction->mark_all_states_as_goals();
}

void CEGAR::refine_abstraction(const Flaw& flaw, utils::Timer& timer)
{
    TimerScope scope(timer);
    const AbstractState& abstract_state = flaw.current_abstract_state;
    vector<Split> splits = flaw.get_possible_splits();
    const auto& [var, wanted] =
        split_selector->pick_split(abstract_state, splits);
    refine_abstraction(abstract_state, var, wanted);
}

void CEGAR::refine_abstraction(
    const AbstractState& abstract_state,
    int split_var,
    const std::vector<int>& wanted)
{
    if (log.is_at_least_debug())
        log << "Refine " << abstract_state << " for " << split_var << "="
            << wanted << endl;

    int v_id = abstract_state.get_id();
    // Reuse state ID from obsolete parent to obtain consecutive IDs.
    int v1_id = v_id;
    int v2_id = abstraction->get_num_states();

    // Update refinement hierarchy.
    pair<NodeID, NodeID> node_ids = refinement_hierarchy->split(
        abstract_state.get_node_id(),
        split_var,
        wanted,
        v1_id,
        v2_id);

    pair<cegar::CartesianSet, cegar::CartesianSet> cartesian_sets =
        abstract_state.split_domain(split_var, wanted);

    unique_ptr v1 = std::make_unique<AbstractState>(
        v1_id,
        node_ids.first,
        std::move(cartesian_sets.first));
    unique_ptr v2 = std::make_unique<AbstractState>(
        v2_id,
        node_ids.second,
        std::move(cartesian_sets.second));

    abstraction
        ->refine(abstract_state, std::move(v1), std::move(v2), split_var);

    heuristic.on_split(v_id);
    flaw_generator->notify_split();
}

void CEGAR::refinement_loop()
{
    utils::Timer refine_timer(true);

    /*
      For landmark tasks we have to map all states in which the
      landmark might have been achieved to arbitrary abstract goal
      states. For the other types of subtasks our method won't find
      unreachable facts, but calling it unconditionally for subtasks
      with one goal doesn't hurt and simplifies the implementation.
    */
    if (task_proxy.get_goals().size() == 1) {
        separate_facts_unreachable_before_goal(refine_timer);
    }

    try {
        while (may_keep_refining()) {
            timer.throw_if_expired();

            std::optional flaw = flaw_generator->generate_flaw(
                task_proxy,
                domain_sizes,
                *abstraction,
                cost_function,
                &abstraction->get_initial_state(),
                heuristic,
                log,
                timer);

            if (!utils::extra_memory_padding_is_reserved()) {
                if (log.is_at_least_normal()) {
                    log << "Reached memory limit during flaw search." << endl;
                }
                break;
            }

            if (!flaw) {
                if (log.is_at_least_normal()) {
                    log << "Failed to generate a flaw. Stopping refinement "
                           "loop."
                        << endl;
                }
                break;
            }

            refine_abstraction(*flaw, refine_timer);

            if (log.is_at_least_verbose() &&
                abstraction->get_num_states() % 1000 == 0) {
                log << abstraction->get_num_states() << "/" << max_states
                    << " states, "
                    << abstraction->get_transition_system().get_num_non_loops()
                    << "/" << max_non_looping_transitions << " transitions"
                    << endl;
            }
        }
    } catch (utils::TimeoutException&) {
        // NOTE: The time limit is not checked during abstraction refinement,
        // although this may be an expensive operation, since it cannot be
        // interrupted without corrupting the abstraction.
        if (log.is_at_least_normal()) {
            log << "Reached time limit." << endl;
        }
    }

    flaw_generator->print_statistics(log);

    if (log.is_at_least_normal()) {
        log << "Time for splitting states: " << refine_timer << endl;
    }
}

void CEGAR::print_statistics()
{
    if (log.is_at_least_normal()) {
        abstraction->print_statistics();
    }
}

} // namespace cartesian
} // namespace heuristics
} // namespace probfd