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

struct VarDomainSplit;
class ProbabilisticTransitionSystem;

struct AbstractStateSplit {
    AbstractStateIndex left;
    AbstractStateIndex right;
};

/*
  Store the set of AbstractStates, use FlawGenerator to find flaws,
  use SplitSelector to select splits in case of ambiguities, break spurious
  solutions and maintain the RefinementHierarchy.
*/
class Abstraction
    : public SimpleMDP<AbstractStateIndex, const ProbabilisticTransition*> {
    // All (as of yet unsplit) abstract states.
    AbstractStates states;

    // The transitions of the abstractions.
    const std::unique_ptr<ProbabilisticTransitionSystem> transition_system;

    // Index of the abstract initial state.
    AbstractStateIndex init_id;

    // Abstract goal states.
    Goals goals;

    // Operator cost function
    std::vector<value_t> operator_costs;

    mutable utils::LogProxy log;

public:
    Abstraction(
        ProbabilisticTaskProxy task,
        std::vector<value_t> operator_costs,
        utils::LogProxy log);
    ~Abstraction();

    Abstraction(const Abstraction&) = delete;

    StateID get_state_id(AbstractStateIndex state) override;

    AbstractStateIndex get_state(StateID state_id) override;

    void generate_applicable_actions(
        AbstractStateIndex state,
        std::vector<const ProbabilisticTransition*>& result) override;

    void generate_action_transitions(
        AbstractStateIndex state,
        const ProbabilisticTransition* action,
        Distribution<StateID>& result) override;

    void generate_all_transitions(
        AbstractStateIndex state,
        std::vector<const ProbabilisticTransition*>& aops,
        std::vector<Distribution<StateID>>& successors) override;

    void generate_all_transitions(
        AbstractStateIndex state,
        std::vector<Transition>& transitions) override;

    bool is_goal(AbstractStateIndex state) const override;
    value_t get_non_goal_termination_cost() const override;

    value_t get_action_cost(const ProbabilisticTransition* t) override;

    value_t get_cost(int op_index) const;

    int get_num_states() const;
    const AbstractState& get_initial_state() const;
    const Goals& get_goals() const;
    const AbstractState& get_abstract_state(AbstractStateIndex state_id) const;
    const ProbabilisticTransitionSystem& get_transition_system() const;

    /* Needed for CEGAR::separate_facts_unreachable_before_goal(). */
    void mark_all_states_as_goals();

    // Split state into two child states.
    AbstractStateSplit refine(
        ProbabilisticTaskProxy task_proxy,
        RefinementHierarchy& refinement_hierarchy,
        const AbstractState& abstract_state,
        const VarDomainSplit& split);

    void print_statistics() const override;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_CARTESIAN_ABSTRACTION_H
