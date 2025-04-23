#ifndef PROBFD_CARTESIAN_ABSTRACTIONS_CARTESIAN_ABSTRACTION_H
#define PROBFD_CARTESIAN_ABSTRACTIONS_CARTESIAN_ABSTRACTION_H

#include "probfd/cartesian_abstractions/abstract_state.h"
#include "probfd/cartesian_abstractions/types.h"

#include "probfd/mdp.h"
#include "probfd/state_id.h"
#include "probfd/type_traits.h"
#include "probfd/value_type.h"

#include "downward/utils/logging.h"

#include "downward/abstract_task.h"
#include "downward/state.h"

#include <memory>
#include <utility>
#include <vector>

// Forward Declarations
namespace probfd {
class ProbabilisticTaskProxy;
template <typename>
class Distribution;
} // namespace probfd

namespace probfd::cartesian_abstractions {
class ProbabilisticTransitionSystem;
struct ProbabilisticTransition;
} // namespace probfd::cartesian_abstractions

namespace probfd::cartesian_abstractions {

/*
  Store the set of AbstractStates, use FlawGenerator to find flaws,
  use SplitSelector to select splits in case of ambiguities, break spurious
  solutions and maintain the RefinementHierarchy.
*/
class CartesianAbstraction
    : public SimpleMDP<int, const ProbabilisticTransition*> {
    const std::unique_ptr<ProbabilisticTransitionSystem> transition_system_;
    const downward::State concrete_initial_state_;
    const std::vector<downward::FactPair> goal_facts_;

    // All (yet unsplit) abstract states.
    AbstractStates states_;
    // State ID of abstract initial state.
    int init_id_;
    // Abstract goal states.
    Goals goals_;

    std::vector<value_t> operator_costs_;

    mutable downward::utils::LogProxy log_;

    void initialize_trivial_abstraction(const std::vector<int>& domain_sizes);

public:
    CartesianAbstraction(
        const ProbabilisticTask& task,
        std::vector<value_t> operator_costs,
        downward::utils::LogProxy log);
    ~CartesianAbstraction() override;

    CartesianAbstraction(const CartesianAbstraction&) = delete;

    StateID get_state_id(int state) override;

    int get_state(StateID state_id) override;

    void generate_applicable_actions(
        int state,
        std::vector<const ProbabilisticTransition*>& result) override;

    void generate_action_transitions(
        int state,
        const ProbabilisticTransition* action,
        SuccessorDistribution& successor_dist) override;

    void generate_all_transitions(
        int state,
        std::vector<const ProbabilisticTransition*>& aops,
        std::vector<SuccessorDistribution>& successor_dists) override;

    void generate_all_transitions(
        int state,
        std::vector<TransitionTailType>& transitions) override;

    bool is_goal(int state) const override;
    value_t get_goal_termination_cost() const override;
    value_t get_non_goal_termination_cost() const override;

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

} // namespace probfd::cartesian_abstractions

#endif // PROBFD_CARTESIAN_ABSTRACTIONS_CARTESIAN_ABSTRACTION_H
