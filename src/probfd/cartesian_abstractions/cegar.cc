#include "probfd/cartesian_abstractions/cegar.h"

#include "probfd/cartesian_abstractions/abstract_state.h"
#include "probfd/cartesian_abstractions/cartesian_abstraction.h"
#include "probfd/cartesian_abstractions/flaw.h"
#include "probfd/cartesian_abstractions/flaw_generator.h"
#include "probfd/cartesian_abstractions/heuristics.h"
#include "probfd/cartesian_abstractions/probabilistic_transition_system.h"
#include "probfd/cartesian_abstractions/split_selector.h"
#include "probfd/cartesian_abstractions/utils.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/utils/guards.h"

#include "probfd/probabilistic_task.h"
#include "probfd/task_proxy.h"

#include "downward/cartesian_abstractions/refinement_hierarchy.h"
#include "downward/cartesian_abstractions/utils.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/exceptions.h"
#include "downward/utils/hash.h"
#include "downward/utils/memory.h"
#include "downward/utils/timer.h"

#include "downward/task_proxy.h"

#include <cassert>
#include <iostream>
#include <utility>

using namespace std;
using namespace downward;
using namespace downward::utils;

namespace probfd::cartesian_abstractions {

CEGARResult::~CEGARResult() = default;

CEGAR::CEGAR(
    int max_states,
    int max_non_looping_transitions,
    double max_time,
    std::shared_ptr<FlawGeneratorFactory> flaw_generator_factory,
    std::shared_ptr<SplitSelectorFactory> split_selector_factory,
    LogProxy log)
    : max_states_(max_states)
    , max_non_looping_transitions_(max_non_looping_transitions)
    , max_time_(max_time)
    , flaw_generator_factory_(std::move(flaw_generator_factory))
    , split_selector_factory_(std::move(split_selector_factory))
    , log_(std::move(log))
{
    assert(max_states_ >= 1);
}

CEGAR::~CEGAR() = default;

CEGARResult
CEGAR::run_refinement_loop(const shared_ptr<ProbabilisticTask>& task)
{
    if (log_.is_at_least_normal()) {
        log_ << "Start building abstraction." << endl;
        log_ << "Maximum number of abstract states: " << max_states_ << endl;
        log_ << "Maximum number of abstract transitions: "
             << max_non_looping_transitions_ << endl;
    }

    const ProbabilisticTaskProxy task_proxy(*task);
    const std::vector<int> domain_sizes(
        ::cartesian_abstractions::get_domain_sizes(task_proxy));

    std::unique_ptr<FlawGenerator> flaw_generator =
        flaw_generator_factory_->create_flaw_generator();
    std::unique_ptr<SplitSelector> split_selector =
        split_selector_factory_->create_split_selector(task);

    // Limit the time for building the abstraction.
    CountdownTimer timer(max_time_);

    /* DAG with inner nodes for all split states and leaves for all
       current states. */
    std::unique_ptr<RefinementHierarchy> refinement_hierarchy(
        new RefinementHierarchy());
    std::unique_ptr<CartesianAbstraction> abstraction(new CartesianAbstraction(
        task_proxy,
        task_properties::get_operator_costs(task_proxy),
        log_));
    std::unique_ptr<CartesianHeuristic> heuristic(new CartesianHeuristic());

    Timer refine_timer(true);

    /*
      For landmark tasks we have to map all states in which the
      landmark might have been achieved to arbitrary abstract goal
      states. For the other types of subtasks our method won't find
      unreachable facts, but calling it unconditionally for subtasks
      with one goal doesn't hurt and simplifies the implementation.
    */
    if (task_proxy.get_goals().size() == 1) {
        separate_facts_unreachable_before_goal(
            task_proxy,
            *flaw_generator,
            *refinement_hierarchy,
            *abstraction,
            *heuristic,
            refine_timer);
    }

    try {
        while (may_keep_refining(*abstraction)) {
            timer.throw_if_expired();

            std::optional flaw = flaw_generator->generate_flaw(
                task_proxy,
                domain_sizes,
                *abstraction,
                &abstraction->get_initial_state(),
                *heuristic,
                log_,
                timer);

            if (!extra_memory_padding_is_reserved()) {
                if (log_.is_at_least_normal()) {
                    log_ << "Reached memory limit during flaw search." << endl;
                }
                break;
            }

            if (!flaw) {
                if (log_.is_at_least_normal()) {
                    log_ << "Failed to generate a flaw. Stopping refinement "
                            "loop."
                         << endl;
                }
                break;
            }

            refine_abstraction(
                *flaw_generator,
                *split_selector,
                *refinement_hierarchy,
                *abstraction,
                *heuristic,
                *flaw,
                refine_timer);

            if (log_.is_at_least_verbose() &&
                abstraction->get_num_states() % 1000 == 0) {
                log_ << abstraction->get_num_states() << "/" << max_states_
                     << " states, "
                     << abstraction->get_transition_system().get_num_non_loops()
                     << "/" << max_non_looping_transitions_ << " transitions"
                     << endl;
            }
        }
    } catch (TimeoutException&) {
        // NOTE: The time limit is not checked during abstraction refinement,
        // although this may be an expensive operation, since it cannot be
        // interrupted without corrupting the abstraction.
        if (log_.is_at_least_normal()) {
            log_ << "Reached time limit." << endl;
        }
    }

    flaw_generator->print_statistics(log_);

    if (log_.is_at_least_normal()) {
        log_ << "Time for splitting states: " << refine_timer << endl;
    }

    if (log_.is_at_least_normal()) {
        log_ << "Done building abstraction." << endl;
        log_ << "Time for building abstraction: " << timer.get_elapsed_time()
             << endl;
        abstraction->print_statistics();
    }

    return CEGARResult{
        std::move(refinement_hierarchy),
        std::move(abstraction),
        std::move(heuristic)};
}

bool CEGAR::may_keep_refining(const CartesianAbstraction& abstraction) const
{
    if (abstraction.get_num_states() >= max_states_) {
        if (log_.is_at_least_normal()) {
            log_ << "Reached maximum number of states." << endl;
        }
        return false;
    } else if (
        abstraction.get_transition_system().get_num_non_loops() >=
        max_non_looping_transitions_) {
        if (log_.is_at_least_normal()) {
            log_ << "Reached maximum number of transitions." << endl;
        }
        return false;
    } else if (!extra_memory_padding_is_reserved()) {
        if (log_.is_at_least_normal()) {
            log_ << "Reached memory limit." << endl;
        }
        return false;
    }
    return true;
}

void CEGAR::separate_facts_unreachable_before_goal(
    ProbabilisticTaskProxy task_proxy,
    FlawGenerator& flaw_generator,
    RefinementHierarchy& refinement_hierarchy,
    CartesianAbstraction& abstraction,
    CartesianHeuristic& heuristic,
    Timer& timer)
{
    assert(abstraction.get_goals().size() == 1);
    assert(abstraction.get_num_states() == 1);
    assert(task_proxy.get_goals().size() == 1);

    FactPair goal = task_proxy.get_goals()[0].get_pair();
    HashSet<FactPair> reachable_facts =
        get_relaxed_possible_before(task_proxy, goal);
    for (VariableProxy var : task_proxy.get_variables()) {
        if (!may_keep_refining(abstraction)) break;
        int var_id = var.get_id();
        vector<int> unreachable_values;
        for (int value = 0; value < var.get_domain_size(); ++value) {
            FactPair fact = var.get_fact(value).get_pair();
            if (!reachable_facts.contains(fact))
                unreachable_values.push_back(value);
        }
        if (!unreachable_values.empty()) {
            TimerScope scope(timer);
            refine_abstraction(
                flaw_generator,
                refinement_hierarchy,
                abstraction,
                heuristic,
                abstraction.get_initial_state(),
                var_id,
                unreachable_values);
        }
    }
    abstraction.mark_all_states_as_goals();
}

void CEGAR::refine_abstraction(
    FlawGenerator& flaw_generator,
    SplitSelector& split_selector,
    RefinementHierarchy& refinement_hierarchy,
    CartesianAbstraction& abstraction,
    CartesianHeuristic& heuristic,
    const Flaw& flaw,
    Timer& timer)
{
    TimerScope scope(timer);
    const AbstractState& abstract_state = flaw.current_abstract_state;
    vector<Split> splits = flaw.get_possible_splits();
    const auto& [var, wanted] =
        split_selector.pick_split(abstract_state, splits);
    refine_abstraction(
        flaw_generator,
        refinement_hierarchy,
        abstraction,
        heuristic,
        abstract_state,
        var,
        wanted);
}

void CEGAR::refine_abstraction(
    FlawGenerator& flaw_generator,
    RefinementHierarchy& refinement_hierarchy,
    CartesianAbstraction& abstraction,
    CartesianHeuristic& heuristic,
    const AbstractState& abstract_state,
    int split_var,
    const std::vector<int>& wanted)
{
    int id = abstract_state.get_id();
    abstraction.refine(refinement_hierarchy, abstract_state, split_var, wanted);
    heuristic.on_split(id);
    flaw_generator.notify_split();
}

} // namespace probfd::cartesian_abstractions
