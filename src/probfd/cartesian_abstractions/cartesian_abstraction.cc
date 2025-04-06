#include "probfd/cartesian_abstractions/cartesian_abstraction.h"

#include "probfd/cartesian_abstractions/abstract_state.h"
#include "probfd/cartesian_abstractions/probabilistic_transition.h"
#include "probfd/cartesian_abstractions/probabilistic_transition_system.h"

#include "probfd/distribution.h"
#include "probfd/task_proxy.h"
#include "probfd/transition_tail.h"

#include "downward/cartesian_abstractions/refinement_hierarchy.h"

#include "downward/cartesian_abstractions/cartesian_set.h"

#include "downward/task_utils/task_properties.h"

#include <cassert>
#include <iostream>

using namespace std;
using namespace downward;

namespace probfd::cartesian_abstractions {

namespace {
vector<int> get_domain_sizes(const PlanningTaskProxy& task)
{
    vector<int> domain_sizes;
    for (VariableProxy var : task.get_variables())
        domain_sizes.push_back(var.get_domain_size());
    return domain_sizes;
}
} // namespace

CartesianAbstraction::CartesianAbstraction(
    const ProbabilisticTaskProxy& task_proxy,
    std::vector<value_t> operator_costs,
    utils::LogProxy log)
    : transition_system_(std::make_unique<ProbabilisticTransitionSystem>(
          task_proxy.get_operators()))
    , concrete_initial_state_(task_proxy.get_initial_state())
    , goal_facts_(::task_properties::get_fact_pairs(task_proxy.get_goals()))
    , operator_costs_(std::move(operator_costs))
    , log_(std::move(log))
{
    initialize_trivial_abstraction(get_domain_sizes(task_proxy));
}

CartesianAbstraction::~CartesianAbstraction() = default;

StateID CartesianAbstraction::get_state_id(int state)
{
    return static_cast<StateID>(state);
}

int CartesianAbstraction::get_state(StateID state_id)
{
    return static_cast<int>(state_id);
}

void CartesianAbstraction::generate_applicable_actions(
    int state,
    std::vector<const ProbabilisticTransition*>& result)
{
    for (const auto* t :
         transition_system_->get_outgoing_transitions()[state]) {
        result.push_back(t);
    }
}

void CartesianAbstraction::generate_action_transitions(
    int source,
    const ProbabilisticTransition* action,
    SuccessorDistribution& successor_dist)
{
    successor_dist.non_source_probability = 0_vt;

    for (size_t i = 0; i != action->target_ids.size(); ++i) {
        const auto succ = action->target_ids[i];

        if (source == succ) continue;

        const value_t probability =
            transition_system_->get_probability(action->op_id, i);
        successor_dist.add_non_source_probability(succ, probability);
    }
}

void CartesianAbstraction::generate_all_transitions(
    int state,
    std::vector<const ProbabilisticTransition*>& aops,
    std::vector<SuccessorDistribution>& successor_dists)
{
    for (const auto* t :
         transition_system_->get_outgoing_transitions()[state]) {
        aops.push_back(t);
        generate_action_transitions(state, t, successor_dists.emplace_back());
    }
}

void CartesianAbstraction::generate_all_transitions(
    int state,
    std::vector<TransitionTailType>& transitions)
{
    for (const auto* t :
         transition_system_->get_outgoing_transitions()[state]) {
        TransitionTailType& transition = transitions.emplace_back(t);
        generate_action_transitions(state, t, transition.successor_dist);
    }
}

bool CartesianAbstraction::is_goal(int state) const
{
    return goals_.contains(state);
}

value_t CartesianAbstraction::get_goal_termination_cost() const
{
    return 0_vt;
}

value_t CartesianAbstraction::get_non_goal_termination_cost() const
{
    return INFINITE_VALUE;
}

value_t CartesianAbstraction::get_action_cost(const ProbabilisticTransition* t)
{
    return operator_costs_[t->op_id];
}

value_t CartesianAbstraction::get_cost(int op_index) const
{
    return operator_costs_[op_index];
}

const AbstractState& CartesianAbstraction::get_initial_state() const
{
    return *states_[init_id_];
}

int CartesianAbstraction::get_num_states() const
{
    return static_cast<int>(states_.size());
}

const Goals& CartesianAbstraction::get_goals() const
{
    return goals_;
}

const AbstractState&
CartesianAbstraction::get_abstract_state(int state_id) const
{
    return *states_[state_id];
}

const ProbabilisticTransitionSystem&
CartesianAbstraction::get_transition_system() const
{
    return *transition_system_;
}

void CartesianAbstraction::mark_all_states_as_goals()
{
    goals_.clear();
    for (auto& state : states_) {
        goals_.insert(state->get_id());
    }
}

void CartesianAbstraction::initialize_trivial_abstraction(
    const vector<int>& domain_sizes)
{
    unique_ptr<AbstractState> init_state =
        AbstractState::get_trivial_abstract_state(domain_sizes);
    init_id_ = init_state->get_id();
    goals_.insert(init_state->get_id());
    states_.push_back(std::move(init_state));
}

pair<int, int> CartesianAbstraction::refine(
    RefinementHierarchy& refinement_hierarchy,
    const AbstractState& abstract_state,
    int split_var,
    const std::vector<int>& wanted)
{
    if (log_.is_at_least_debug())
        log_ << "Refine " << abstract_state << " for " << split_var << "="
             << wanted << endl;

    int v_id = abstract_state.get_id();
    // Reuse state ID from obsolete parent to obtain consecutive IDs.
    int v1_id = v_id;
    int v2_id = get_num_states();

    // Update refinement hierarchy.
    pair<NodeID, NodeID> node_ids = refinement_hierarchy.split(
        abstract_state.get_node_id(),
        split_var,
        wanted,
        v1_id,
        v2_id);

    pair<
        cartesian_abstractions::CartesianSet,
        cartesian_abstractions::CartesianSet>
        cartesian_sets = abstract_state.split_domain(split_var, wanted);

    unique_ptr v1 = std::make_unique<AbstractState>(
        v1_id,
        node_ids.first,
        std::move(cartesian_sets.first));
    unique_ptr v2 = std::make_unique<AbstractState>(
        v2_id,
        node_ids.second,
        std::move(cartesian_sets.second));

    assert(abstract_state.includes(*v1));
    assert(abstract_state.includes(*v2));

    /*
      Due to the way we split the state into v1 and v2, v2 is never the new
      initial state and v1 is never a goal state.
    */
    if (abstract_state.get_id() == init_id_) {
        if (v1->includes(concrete_initial_state_)) {
            assert(!v2->includes(concrete_initial_state_));
            init_id_ = v1_id;
        } else {
            assert(v2->includes(concrete_initial_state_));
            init_id_ = v2_id;
        }
        if (log_.is_at_least_debug()) {
            log_ << "New init state #" << init_id_ << ": "
                 << get_state(init_id_) << endl;
        }
    }
    if (goals_.count(v1_id)) {
        goals_.erase(v1_id);
        if (v1->includes(goal_facts_)) {
            goals_.insert(v1_id);
        }
        if (v2->includes(goal_facts_)) {
            goals_.insert(v2_id);
        }
        if (log_.is_at_least_debug()) {
            log_ << "Goal states: " << goals_.size() << endl;
        }
    }

    transition_system_->rewire(states_, *v1, *v2, split_var);

    states_[v1_id] = std::move(v1);
    assert(static_cast<int>(states_.size()) == v2_id);
    states_.push_back(std::move(v2));

    return {v1_id, v2_id};
}

void CartesianAbstraction::print_statistics() const
{
    if (log_.is_at_least_normal()) {
        log_ << "States: " << get_num_states() << endl;
        log_ << "Goal states: " << goals_.size() << endl;
        transition_system_->print_statistics(log_);
    }
}

} // namespace probfd::cartesian_abstractions
