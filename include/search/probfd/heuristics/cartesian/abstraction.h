#ifndef PROBFD_HEURISTICS_CARTESIAN_ABSTRACTION_H
#define PROBFD_HEURISTICS_CARTESIAN_ABSTRACTION_H

#include "probfd/heuristics/cartesian/types.h"

#include "probfd/mdp.h"
#include "probfd/task_proxy.h"

#include "downward/utils/collections.h"
#include "downward/utils/logging.h"

#include <memory>
#include <vector>

namespace utils {
class CountdownTimer;
class LogProxy;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace cartesian {

class ProbabilisticTransitionSystem;

/*
  Store the set of AbstractStates, use FlawGenerator to find flaws,
  use SplitSelector to select splits in case of ambiguities, break spurious
  solutions and maintain the RefinementHierarchy.
*/
class Abstraction
    : public MDP<const AbstractState*, const ProbabilisticTransition*> {
    const std::unique_ptr<ProbabilisticTransitionSystem> transition_system;
    const State concrete_initial_state;
    const std::vector<FactPair> goal_facts;

    // All (as of yet unsplit) abstract states.
    AbstractStates states;
    // State ID of abstract initial state.
    int init_id;
    // Abstract goal states.
    Goals goals;

    std::vector<value_t> operator_costs;

    mutable utils::LogProxy log;

    void initialize_trivial_abstraction(const std::vector<int>& domain_sizes);

public:
    Abstraction(
        const ProbabilisticTaskProxy& task,
        std::vector<value_t> operator_costs,
        utils::LogProxy log);
    ~Abstraction();

    Abstraction(const Abstraction&) = delete;

    StateID get_state_id(const AbstractState* state) override;

    const AbstractState* get_state(StateID state_id) override;

    void generate_applicable_actions(
        StateID state,
        std::vector<const ProbabilisticTransition*>& result) override;

    void generate_action_transitions(
        StateID,
        const ProbabilisticTransition* action,
        Distribution<StateID>& result) override;

    void generate_all_transitions(
        StateID state,
        std::vector<const ProbabilisticTransition*>& aops,
        std::vector<Distribution<StateID>>& successors) override;

    void generate_all_transitions(
        StateID state,
        std::vector<Transition<const ProbabilisticTransition*>>& transitions)
        override;

    TerminationInfo get_termination_info(const AbstractState* s) override;

    value_t get_action_cost(const ProbabilisticTransition* t) override;

    value_t get_cost(int op_index) const;

    int get_num_states() const;
    const AbstractState& get_initial_state() const;
    const Goals& get_goals() const;
    const AbstractState& get_abstract_state(int state_id) const;
    const ProbabilisticTransitionSystem& get_transition_system() const;

    /* Needed for CEGAR::separate_facts_unreachable_before_goal(). */
    void mark_all_states_as_goals();

    // Split state into two child states.
    std::pair<int, int> refine(
        RefinementHierarchy& refinement_hierarchy,
        const AbstractState& abstract_state,
        int split_var,
        const std::vector<int>& wanted);

    void print_statistics() const override;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
