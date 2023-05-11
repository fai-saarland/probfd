#include "probfd/heuristics/cartesian/abstraction.h"

#include "probfd/heuristics/cartesian/abstract_state.h"
#include "probfd/heuristics/cartesian/probabilistic_transition.h"
#include "probfd/heuristics/cartesian/probabilistic_transition_system.h"

#include "probfd/task_utils/task_properties.h"

#include "cegar/refinement_hierarchy.h"

#include "utils/countdown_timer.h"
#include "utils/logging.h"
#include "utils/math.h"
#include "utils/memory.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <unordered_map>

using namespace std;

namespace probfd {
namespace heuristics {
namespace cartesian {

namespace {
vector<int> get_domain_sizes(const TaskBaseProxy& task)
{
    vector<int> domain_sizes;
    for (VariableProxy var : task.get_variables())
        domain_sizes.push_back(var.get_domain_size());
    return domain_sizes;
}
} // namespace

Abstraction::Abstraction(
    const shared_ptr<ProbabilisticTask>& task,
    utils::LogProxy& log)
    : transition_system(std::make_unique<ProbabilisticTransitionSystem>(
          ProbabilisticTaskProxy(*task).get_operators()))
    , concrete_initial_state(ProbabilisticTaskProxy(*task).get_initial_state())
    , goal_facts(::task_properties::get_fact_pairs(
          ProbabilisticTaskProxy(*task).get_goals()))
    , refinement_hierarchy(std::make_unique<RefinementHierarchy>(task))
    , log(log)
{
    initialize_trivial_abstraction(
        get_domain_sizes(ProbabilisticTaskProxy(*task)));
}

Abstraction::~Abstraction() = default;

StateID
Abstraction::get_state_id(const heuristics::cartesian::AbstractState* state)
{
    return state->get_id();
}

const heuristics::cartesian::AbstractState*
Abstraction::get_state(StateID state_id)
{
    return &get_abstract_state(state_id.id);
}

ActionID Abstraction::get_action_id(
    StateID,
    const heuristics::cartesian::ProbabilisticTransition* action)
{
    return ActionID(reinterpret_cast<uintptr_t>(action));
}

const heuristics::cartesian::ProbabilisticTransition*
Abstraction::get_action(StateID, ActionID action_id)
{
    return reinterpret_cast<
        const heuristics::cartesian::ProbabilisticTransition*>(
        static_cast<uintptr_t>(action_id.id));
}

void Abstraction::generate_applicable_actions(
    StateID state,
    std::vector<const heuristics::cartesian::ProbabilisticTransition*>& result)
{
    for (const auto* t :
         transition_system->get_outgoing_transitions()[state.id]) {
        result.push_back(t);
    }
}

void Abstraction::generate_action_transitions(
    StateID,
    const heuristics::cartesian::ProbabilisticTransition* action,
    Distribution<StateID>& result)
{
    for (size_t i = 0; i != action->target_ids.size(); ++i) {
        const value_t probability =
            transition_system->get_probability(action->op_id, i);
        result.add_probability(action->target_ids[i], probability);
    }
}

void Abstraction::generate_all_transitions(
    StateID state,
    std::vector<const heuristics::cartesian::ProbabilisticTransition*>& aops,
    std::vector<Distribution<StateID>>& successors)
{
    for (const auto* t :
         transition_system->get_outgoing_transitions()[state.id]) {
        aops.push_back(t);
        generate_action_transitions(state, t, successors.emplace_back());
    }
}

const AbstractState& Abstraction::get_initial_state() const
{
    return *states[init_id];
}

int Abstraction::get_num_states() const
{
    return states.size();
}

const Goals& Abstraction::get_goals() const
{
    return goals;
}

const AbstractState& Abstraction::get_abstract_state(int state_id) const
{
    return *states[state_id];
}

const ProbabilisticTransitionSystem& Abstraction::get_transition_system() const
{
    return *transition_system;
}

unique_ptr<RefinementHierarchy> Abstraction::extract_refinement_hierarchy()
{
    assert(refinement_hierarchy);
    return std::move(refinement_hierarchy);
}

void Abstraction::mark_all_states_as_goals()
{
    goals.clear();
    for (auto& state : states) {
        goals.insert(state->get_id());
    }
}

void Abstraction::initialize_trivial_abstraction(
    const vector<int>& domain_sizes)
{
    unique_ptr<AbstractState> init_state =
        AbstractState::get_trivial_abstract_state(domain_sizes);
    init_id = init_state->get_id();
    goals.insert(init_state->get_id());
    states.push_back(std::move(init_state));
}

pair<int, int> Abstraction::refine(
    const AbstractState& state,
    int var,
    const vector<int>& wanted)
{
    if (log.is_at_least_debug())
        log << "Refine " << state << " for " << var << "=" << wanted << endl;

    int v_id = state.get_id();
    // Reuse state ID from obsolete parent to obtain consecutive IDs.
    int v1_id = v_id;
    int v2_id = get_num_states();

    // Update refinement hierarchy.
    pair<NodeID, NodeID> node_ids =
        refinement_hierarchy
            ->split(state.get_node_id(), var, wanted, v1_id, v2_id);

    pair<cegar::CartesianSet, cegar::CartesianSet> cartesian_sets =
        state.split_domain(var, wanted);

    unique_ptr<AbstractState> v1 = std::make_unique<AbstractState>(
        v1_id,
        node_ids.first,
        std::move(cartesian_sets.first));
    unique_ptr<AbstractState> v2 = std::make_unique<AbstractState>(
        v2_id,
        node_ids.second,
        std::move(cartesian_sets.second));
    assert(state.includes(*v1));
    assert(state.includes(*v2));

    /*
      Due to the way we split the state into v1 and v2, v2 is never the new
      initial state and v1 is never a goal state.
    */
    if (state.get_id() == init_id) {
        if (v1->includes(concrete_initial_state)) {
            assert(!v2->includes(concrete_initial_state));
            init_id = v1_id;
        } else {
            assert(v2->includes(concrete_initial_state));
            init_id = v2_id;
        }
        if (log.is_at_least_debug()) {
            log << "New init state #" << init_id << ": " << get_state(init_id)
                << endl;
        }
    }
    if (goals.count(v_id)) {
        goals.erase(v_id);
        if (v1->includes(goal_facts)) {
            goals.insert(v1_id);
        }
        if (v2->includes(goal_facts)) {
            goals.insert(v2_id);
        }
        if (log.is_at_least_debug()) {
            log << "Goal states: " << goals.size() << endl;
        }
    }

    transition_system->rewire(states, *v1, *v2, var);

    states[v1_id] = std::move(v1);
    assert(static_cast<int>(states.size()) == v2_id);
    states.push_back(std::move(v2));

    return {v1_id, v2_id};
}

void Abstraction::print_statistics() const
{
    if (log.is_at_least_normal()) {
        log << "States: " << get_num_states() << endl;
        log << "Goal states: " << goals.size() << endl;
        transition_system->print_statistics(log);
    }
}

} // namespace cartesian
} // namespace heuristics
} // namespace probfd