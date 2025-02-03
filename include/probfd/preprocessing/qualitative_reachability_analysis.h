#ifndef PROBFD_PREPROCESSING_QUALITATIVE_REACHABILITY_ANALYSIS_H
#define PROBFD_PREPROCESSING_QUALITATIVE_REACHABILITY_ANALYSIS_H

#include "probfd/quotients/quotient_system.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/utils/iterators.h"

#include "probfd/heuristic.h"
#include "probfd/mdp.h"
#include "probfd/type_traits.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/timer.h"

#include <cassert>
#include <deque>
#include <iostream>
#include <iterator>
#include <limits>
#include <type_traits>
#include <vector>

namespace probfd::preprocessing {

struct QRStatistics {
    unsigned long long goals = 0;
    unsigned long long terminals = 0;
    unsigned long long selfloops = 0;

    unsigned long long sccs1 = 0;
    unsigned long long sccsk = 0;
    unsigned long long sccs1_dead = 0;
    unsigned long long sccsk_dead = 0;
    unsigned long long ones = 0;

    utils::Timer time;

    void print(std::ostream& out) const;
};

namespace internal {

struct StateInfo {
    enum { NEW, ONSTACK, CLOSED };
    static constexpr uint32_t UNDEF = std::numeric_limits<uint32_t>::max() >> 3;

    unsigned explored : 1 = 0;
    unsigned dead : 1 = 1;     // dead end flag
    unsigned solvable : 1 = 0; // solvable state flag
    unsigned stackid : 29 = UNDEF;

    [[nodiscard]]
    auto get_status() const;
};

struct StackInfo {
    struct ParentTransition {
        unsigned parent_idx;
        unsigned parent_transition_idx;
    };

    // A transition is active if it is not going to a state with goal
    // probability less than one. This information is iteratively refined in a
    // fixpoint iteration as more states with this property are found.

    struct TransitionFlags {
        bool is_active_exiting : 1; // Is the transition active and an SCC exit?
        bool is_active : 1;         // Is the transition active?
    };

    StateID stateid;

    unsigned active_exit_transitions = 0; // Number of active exit transitions.
    unsigned active_transitions = 0;      // Number of active transitions.

    std::vector<TransitionFlags> transition_flags;
    std::vector<ParentTransition> parents;

    explicit StackInfo(StateID sid);
};

} // namespace internal

/**
 * @brief Algorithm that computes all dead-ends and states with a goal
 * probability of one which are reachable from a given initial state.
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename State, typename Action>
class QualitativeReachabilityAnalysis {
    using MDPType = MDP<State, Action>;
    using EvaluatorType = Heuristic<State>;

    using StateInfo = internal::StateInfo;
    using StackInfo = internal::StackInfo;

    struct ExpansionInfo {
        StateID state_id;
        StackInfo& stack_info;
        StateInfo& state_info;

        // Tarjan's SCC algorithm: stack id and lowlink
        unsigned stck;
        unsigned lstck;

        // Temporary transition flags
        bool exits_only_solvable : 1 = true;
        bool exits_scc : 1 = false;
        bool transitions_in_scc : 1 = false;

        // Mutable info
        std::vector<Action> aops;         // Remaining unexpanded operators
        Distribution<StateID> transition; // Currently expanded transition
        // Next state to expand
        typename Distribution<StateID>::const_iterator successor;

        explicit ExpansionInfo(
            StateID state_id,
            StackInfo& stack_info,
            StateInfo& state_info,
            unsigned int stck);

        /**
         * Advances to the next non-loop action. Returns nullptr if such an
         * action does not exist.
         */
        bool next_action(MDPType& mdp);
        bool forward_non_self_loop(MDPType& mdp, const State& state);
        bool next_successor();

        StateID get_current_successor();
    };

    const bool expand_goals_;

    storage::PerStateStorage<StateInfo> state_infos_;
    std::deque<ExpansionInfo> expansion_queue_;
    std::deque<StackInfo> stack_;

    QRStatistics stats_;

public:
    explicit QualitativeReachabilityAnalysis(bool expand_goals);

    void run_analysis(
        MDPType& mdp,
        const EvaluatorType* pruning_function,
        param_type<State> source_state,
        std::output_iterator<StateID> auto dead_out,
        std::output_iterator<StateID> auto unsolvable_out,
        std::output_iterator<StateID> auto solvable_out,
        double max_time = std::numeric_limits<double>::infinity());

private:
    void push_state(StateID state_id, StateInfo& state_info);

    bool initialize(
        MDPType& mdp,
        const EvaluatorType* pruning_function,
        ExpansionInfo& exp_info);

    bool push_successor(
        MDPType& mdp,
        ExpansionInfo& e,
        utils::CountdownTimer& timer);

    void scc_found(
        unsigned int stack_idx,
        std::output_iterator<StateID> auto dead_out,
        std::output_iterator<StateID> auto unsolvable_out,
        std::output_iterator<StateID> auto solvable_out,
        utils::CountdownTimer& timer);
};

} // namespace probfd::preprocessing

#define GUARD_INCLUDE_PROBFD_PREPROCESSING_QUALITATIVE_REACHABILITY_ANALYSIS_H
#include "probfd/preprocessing/qualitative_reachability_analysis_impl.h"
#undef GUARD_INCLUDE_PROBFD_PREPROCESSING_QUALITATIVE_REACHABILITY_ANALYSIS_H

#endif // PROBFD_PREPROCESSING_QUALITATIVE_REACHABILITY_ANALYSIS_H