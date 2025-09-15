#include "downward/cartesian_abstractions/cegar.h"

#include "downward/cartesian_abstractions/abstract_state.h"
#include "downward/cartesian_abstractions/abstraction.h"
#include "downward/cartesian_abstractions/cartesian_set.h"
#include "downward/cartesian_abstractions/transition_system.h"
#include "downward/cartesian_abstractions/utils.h"

#include "downward/task_utils/task_properties.h"

#include "downward/utils/logging.h"
#include "downward/utils/math.h"
#include "downward/utils/memory.h"

#include "downward/abstract_task.h"
#include "downward/axioms.h"
#include "downward/initial_state_values.h"

#include <algorithm>
#include <cassert>
#include <downward/operator_cost_function_fwd.h>
#include <iostream>
#include <unordered_map>

using namespace std;

namespace downward::cartesian_abstractions {
// Create the Cartesian set that corresponds to the given preconditions or
// goals.
template <typename ConditionsProxy>
static CartesianSet get_cartesian_set(
    const vector<int>& domain_sizes,
    const ConditionsProxy& conditions)
{
    CartesianSet cartesian_set(domain_sizes);
    for (const auto [var, value] : conditions) {
        cartesian_set.set_single_value(var, value);
    }
    return cartesian_set;
}

struct Flaw {
    // Last concrete and abstract state reached while tracing solution.
    State concrete_state;
    const AbstractState& current_abstract_state;
    // Hypothetical Cartesian set we would have liked to reach.
    CartesianSet desired_cartesian_set;

    Flaw(
        State&& concrete_state,
        const AbstractState& current_abstract_state,
        CartesianSet&& desired_cartesian_set)
        : concrete_state(std::move(concrete_state))
        , current_abstract_state(current_abstract_state)
        , desired_cartesian_set(std::move(desired_cartesian_set))
    {
        assert(current_abstract_state.includes(this->concrete_state));
    }

    vector<Split> get_possible_splits(const VariableSpace& variables) const
    {
        vector<Split> splits;
        /*
          For each fact in the concrete state that is not contained in the
          desired abstract state, loop over all values in the domain of the
          corresponding variable. The values that are in both the current and
          the desired abstract state are the "wanted" ones, i.e., the ones that
          we want to split off.
        */
        for (VariableProxy var : variables) {
            const int var_id = var.get_id();
            const int state_value = concrete_state[var.get_id()];

            if (!desired_cartesian_set.test(var_id, state_value)) {
                vector<int> wanted;
                for (int value = 0; value < var.get_domain_size(); ++value) {
                    if (current_abstract_state.contains(var_id, value) &&
                        desired_cartesian_set.test(var_id, value)) {
                        wanted.push_back(value);
                    }
                }
                assert(!wanted.empty());
                splits.emplace_back(var_id, std::move(wanted));
            }
        }
        assert(!splits.empty());
        return splits;
    }
};

CEGAR::CEGAR(
    SharedAbstractTask task,
    int max_states,
    int max_non_looping_transitions,
    utils::Duration max_time,
    PickSplit pick,
    utils::RandomNumberGenerator& rng,
    utils::LogProxy& log)
    : task_(to_refs(task))
    , axiom_evaluator(
          g_axiom_evaluators
              [get_variables(task_), get_axioms(task_)])
    , domain_sizes(get_domain_sizes(get_variables(task_)))
    , max_states(max_states)
    , max_non_looping_transitions(max_non_looping_transitions)
    , split_selector(std::move(task), pick)
    , abstraction(
          std::make_unique<Abstraction>(
              get_variables(task_),
              get_operators(task_),
              task_properties::get_fact_pairs(get_goal(task_)),
              get_init(task_).get_initial_state(),
              log))
    , abstract_search(
          task_properties::get_operator_costs(
              get_operators(task_),
              get_cost_function(task_)))
    , timer(max_time)
    , log(log)
{
    assert(max_states >= 1);
    if (log.is_at_least_normal()) {
        log << "Start building abstraction." << endl;
        log << "Maximum number of states: " << max_states << endl;
        log << "Maximum number of transitions: " << max_non_looping_transitions
            << endl;
    }

    refinement_loop(rng);
    if (log.is_at_least_normal()) {
        log << "Done building abstraction." << endl;
        log << "Time for building abstraction: " << timer.get_elapsed_time()
            << endl;
        print_statistics();
    }
}

CEGAR::~CEGAR()
{
}

unique_ptr<Abstraction> CEGAR::extract_abstraction()
{
    assert(abstraction);
    return std::move(abstraction);
}

void CEGAR::separate_facts_unreachable_before_goal()
{
    const auto& [variables, operators, goals, init_vals] = slice<
        VariableSpace&,
        ClassicalOperatorSpace&,
        GoalFactList&,
        InitialStateValues&>(task_);

    assert(abstraction->get_goals().size() == 1);
    assert(abstraction->get_num_states() == 1);

    assert(goals.size() == 1);

    FactPair goal = goals[0];
    utils::HashSet<FactPair> reachable_facts = get_relaxed_possible_before(
        operators,
        init_vals.get_initial_state(),
        goal);
    for (VariableProxy var : variables) {
        if (!may_keep_refining()) break;
        int var_id = var.get_id();
        vector<int> unreachable_values;
        for (int value = 0; value < var.get_domain_size(); ++value) {
            FactPair fact(var_id, value);
            if (!reachable_facts.contains(fact))
                unreachable_values.push_back(value);
        }
        if (!unreachable_values.empty())
            abstraction->refine(
                abstraction->get_initial_state(),
                var_id,
                unreachable_values);
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
    } else if (timer.is_expired()) {
        if (log.is_at_least_normal()) { log << "Reached time limit." << endl; }
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
    const auto [variables, goals] = slice<VariableSpace&, GoalFactList&>(task_);

    /*
      For landmark tasks we have to map all states in which the
      landmark might have been achieved to arbitrary abstract goal
      states. For the other types of subtasks our method won't find
      unreachable facts, but calling it unconditionally for subtasks
      with one goal doesn't hurt and simplifies the implementation.
    */
    if (goals.size() == 1) { separate_facts_unreachable_before_goal(); }

    utils::Timer find_trace_timer(false);
    utils::Timer find_flaw_timer(false);
    utils::Timer refine_timer(false);

    while (may_keep_refining()) {
        find_trace_timer.resume();
        unique_ptr<Solution> solution = abstract_search.find_solution(
            abstraction->get_transition_system().get_outgoing_transitions(),
            abstraction->get_initial_state().get_id(),
            abstraction->get_goals());
        find_trace_timer.stop();
        if (!solution) {
            if (log.is_at_least_normal()) {
                log << "Abstract task is unsolvable." << endl;
            }
            break;
        }

        find_flaw_timer.resume();
        unique_ptr<Flaw> flaw = find_flaw(*solution);
        find_flaw_timer.stop();
        if (!flaw) {
            if (log.is_at_least_normal()) {
                log << "Found concrete solution during refinement." << endl;
            }
            break;
        }

        refine_timer.resume();
        const AbstractState& abstract_state = flaw->current_abstract_state;
        int state_id = abstract_state.get_id();
        vector<Split> splits = flaw->get_possible_splits(variables);
        const Split& split =
            split_selector.pick_split(abstract_state, splits, rng);
        auto new_state_ids =
            abstraction->refine(abstract_state, split.var_id, split.values);
        // Since h-values only increase we can assign the h-value to the
        // children.
        abstract_search.copy_h_value_to_children(
            state_id,
            new_state_ids.first,
            new_state_ids.second);
        refine_timer.stop();

        if (log.is_at_least_verbose() &&
            abstraction->get_num_states() % 1000 == 0) {
            log << abstraction->get_num_states() << "/" << max_states
                << " states, "
                << abstraction->get_transition_system().get_num_non_loops()
                << "/" << max_non_looping_transitions << " transitions" << endl;
        }
    }
    if (log.is_at_least_normal()) {
        log << "Time for finding abstract traces: " << find_trace_timer << endl;
        log << "Time for finding flaws: " << find_flaw_timer << endl;
        log << "Time for splitting states: " << refine_timer << endl;
    }
}

unique_ptr<Flaw> CEGAR::find_flaw(const Solution& solution)
{
    const auto& [operators, goals, init_vals] =
        slice<ClassicalOperatorSpace&, GoalFactList&, InitialStateValues&>(
            task_);

    if (log.is_at_least_debug()) log << "Check solution:" << endl;

    const AbstractState* abstract_state = &abstraction->get_initial_state();
    State concrete_state = init_vals.get_initial_state();
    assert(abstract_state->includes(concrete_state));

    if (log.is_at_least_debug())
        log << "  Initial abstract state: " << *abstract_state << endl;

    for (const Transition& step : solution) {
        if (!utils::extra_memory_padding_is_reserved()) break;
        OperatorProxy op = operators[step.op_id];
        const AbstractState* next_abstract_state =
            &abstraction->get_state(step.target_id);
        if (task_properties::is_applicable(op, concrete_state)) {
            if (log.is_at_least_debug())
                log << "  Move to " << *next_abstract_state << " with "
                    << op.get_name() << endl;
            State next_concrete_state =
                concrete_state.get_unregistered_successor(axiom_evaluator, op);
            if (!next_abstract_state->includes(next_concrete_state)) {
                if (log.is_at_least_debug()) log << "  Paths deviate." << endl;
                return std::make_unique<Flaw>(
                    std::move(concrete_state),
                    *abstract_state,
                    next_abstract_state->regress(op));
            }
            abstract_state = next_abstract_state;
            concrete_state = std::move(next_concrete_state);
        } else {
            if (log.is_at_least_debug())
                log << "  Operator not applicable: " << op.get_name() << endl;
            return std::make_unique<Flaw>(
                std::move(concrete_state),
                *abstract_state,
                get_cartesian_set(domain_sizes, op.get_preconditions()));
        }
    }
    assert(abstraction->get_goals().count(abstract_state->get_id()));
    if (task_properties::is_goal_state(goals, concrete_state)) {
        // We found a concrete solution.
        return nullptr;
    } else {
        if (log.is_at_least_debug()) log << "  Goal test failed." << endl;
        return std::make_unique<Flaw>(
            std::move(concrete_state),
            *abstract_state,
            get_cartesian_set(domain_sizes, goals));
    }
}

void CEGAR::print_statistics()
{
    if (log.is_at_least_normal()) {
        abstraction->print_statistics();
        int init_id = abstraction->get_initial_state().get_id();
        log << "Initial h value: " << abstract_search.get_h_value(init_id)
            << endl;
        log << endl;
    }
}
} // namespace downward::cartesian_abstractions
