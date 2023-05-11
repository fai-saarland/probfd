#include "probfd/heuristics/cartesian/cegar.h"

#include "probfd/heuristics/cartesian/abstract_state.h"
#include "probfd/heuristics/cartesian/abstraction.h"
#include "probfd/heuristics/cartesian/probabilistic_transition_system.h"
#include "probfd/heuristics/cartesian/types.h"
#include "probfd/heuristics/cartesian/utils.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/policy.h"

#include "cegar/cartesian_set.h"
#include "cegar/utils.h"

#include "utils/logging.h"
#include "utils/math.h"
#include "utils/memory.h"

#include "probfd/storage/per_state_storage.h"
#include "state_registry.h"

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
    const FlawGeneratorFactory& flaw_generator_factory,
    int max_states,
    int max_search_states,
    int max_non_looping_transitions,
    double max_time,
    PickSplit pick,
    utils::RandomNumberGenerator& rng,
    utils::LogProxy& log)
    : task_proxy(*task)
    , domain_sizes(cegar::get_domain_sizes(task_proxy))
    , max_states(max_states)
    , max_search_states(max_search_states)
    , max_non_looping_transitions(max_non_looping_transitions)
    , split_selector(task, pick)
    , abstraction(new Abstraction(task, log))
    , cost_function(
          *abstraction,
          task_properties::get_operator_costs(task_proxy))
    , flaw_generator(flaw_generator_factory.create_flaw_generator())
    , timer(max_time)
    , log(log)
{
    assert(max_states >= 1);
    if (log.is_at_least_normal()) {
        log << "Start building abstraction." << endl;
        log << "Maximum number of abstract states: " << max_states << endl;
        log << "Maximum number of flaw search states: " << max_search_states
            << endl;
        log << "Maximum number of abstract transitions: "
            << max_non_looping_transitions << endl;
    }

    refinement_loop(rng);
    if (log.is_at_least_normal()) {
        log << "Done building abstraction." << endl;
        log << "Time for building abstraction: " << timer.get_elapsed_time()
            << endl;
        print_statistics();
    }
}

CEGAR::~CEGAR() = default;

unique_ptr<Abstraction> CEGAR::extract_abstraction()
{
    assert(abstraction);
    return std::move(abstraction);
}

CartesianHeuristic& CEGAR::get_heuristic()
{
    return flaw_generator->get_heuristic();
}

void CEGAR::separate_facts_unreachable_before_goal()
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
            abstraction->refine(
                abstraction->get_initial_state(),
                var_id,
                unreachable_values);
            flaw_generator->notify_split(
                abstraction->get_initial_state().get_id());
        }
    }
    abstraction->mark_all_states_as_goals();
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

void CEGAR::refinement_loop(utils::RandomNumberGenerator& rng)
{
    /*
      For landmark tasks we have to map all states in which the
      landmark might have been achieved to arbitrary abstract goal
      states. For the other types of subtasks our method won't find
      unreachable facts, but calling it unconditionally for subtasks
      with one goal doesn't hurt and simplifies the implementation.
    */
    if (task_proxy.get_goals().size() == 1) {
        separate_facts_unreachable_before_goal();
    }

    utils::Timer find_trace_timer(true);
    utils::Timer find_flaw_timer(true);
    utils::Timer refine_timer(true);

    try {
        while (may_keep_refining()) {
            timer.throw_if_expired();

            std::optional flaw = flaw_generator->generate_flaw(
                task_proxy,
                *abstraction,
                cost_function,
                &abstraction->get_initial_state(),
                log,
                domain_sizes,
                find_trace_timer,
                find_flaw_timer,
                timer,
                max_search_states);

            if (!flaw) {
                break;
            }

            if (!utils::extra_memory_padding_is_reserved()) {
                if (log.is_at_least_normal()) {
                    log << "Reached memory limit during flaw search." << endl;
                }
                break;
            }

            refine_timer.resume();
            const AbstractState& abstract_state = flaw->current_abstract_state;
            const int state_id = abstract_state.get_id();
            vector<Split> splits = flaw->get_possible_splits();
            const Split& split =
                split_selector.pick_split(abstract_state, splits, rng);
            abstraction->refine(abstract_state, split.var_id, split.values);
            flaw_generator->notify_split(state_id);
            refine_timer.stop();

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

    if (log.is_at_least_normal()) {
        log << "Time for finding abstract traces: " << find_trace_timer << endl;
        log << "Time for finding flaws: " << find_flaw_timer << endl;
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