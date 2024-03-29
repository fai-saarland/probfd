#ifndef PROBFD_PREPROCESSING_QUALITATIVE_REACHABILITY_ANALYSIS_H
#define PROBFD_PREPROCESSING_QUALITATIVE_REACHABILITY_ANALYSIS_H

#include "probfd/quotients/quotient_system.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/utils/iterators.h"

#include "probfd/evaluator.h"
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

namespace probfd {
namespace preprocessing {

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
    static constexpr uint32_t UNDEF = std::numeric_limits<uint32_t>::max() >> 4;

    unsigned explored : 1 = 0;
    unsigned expandable_goal : 1 = 0; // non-terminal goal?
    unsigned dead : 1 = 1;            // dead end flag
    unsigned one : 1 = 0;             // proper state flag
    unsigned stackid_ : 28 = UNDEF;

    bool onstack() const;
};

struct StackInfo {
    StateID stateid;

    unsigned proper_transitions = 0;
    unsigned exit_transitions = 0;

    std::vector<bool> active;
    std::vector<bool> exiting;
    std::vector<std::pair<unsigned, unsigned>> parents;

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
    using MDP = MDP<State, Action>;
    using Evaluator = Evaluator<State>;

    using StateInfo = internal::StateInfo;
    using StackInfo = internal::StackInfo;

    struct ExpansionInfo {
        // Tarjan's SCC algorithm: stack id and lowlink
        const unsigned stck;
        unsigned lstck;

        bool exits_only_proper = true;
        bool transitions_in_scc = false;
        bool exits_scc = false;

        // Mutable info
        std::vector<Action> aops;         // Remaining unexpanded operators
        Distribution<StateID> transition; // Currently expanded transition
        // Next state to expand
        typename Distribution<StateID>::const_iterator successor;

        explicit ExpansionInfo(
            unsigned stck,
            std::vector<Action> aops,
            Distribution<StateID> transition);

        /**
         * Advances to the next non-loop action. Returns nullptr if such an
         * action does not exist.
         */
        bool next_action(MDP& mdp, StateID state_id);
        bool next_successor();

        StateID get_current_successor();
    };

    const bool expand_goals_;

    storage::PerStateStorage<StateInfo> state_infos_;
    std::deque<ExpansionInfo> expansion_queue_;
    std::vector<StackInfo> stack_;

    QRStatistics stats_;

public:
    explicit QualitativeReachabilityAnalysis(bool expand_goals);

    void run_analysis(
        MDP& mdp,
        const Evaluator* pruning_function,
        param_type<State> source_state,
        std::output_iterator<StateID> auto dead_out,
        std::output_iterator<StateID> auto non_proper_out,
        std::output_iterator<StateID> auto proper_out,
        double max_time = std::numeric_limits<double>::infinity());

private:
    bool push(
        MDP& mdp,
        const Evaluator* pruning_function,
        StateID state_id,
        StateInfo& state_info,
        std::output_iterator<StateID> auto dead_out,
        std::output_iterator<StateID> auto non_proper_out,
        std::output_iterator<StateID> auto proper_out);

    bool push_successor(
        MDP& mdp,
        const Evaluator* pruning_function,
        ExpansionInfo& e,
        StackInfo& s,
        StateInfo& st,
        std::output_iterator<StateID> auto dead_out,
        std::output_iterator<StateID> auto non_proper_out,
        std::output_iterator<StateID> auto proper_out,
        utils::CountdownTimer& timer);

    void scc_found(
        std::ranges::forward_range auto&& scc,
        std::output_iterator<StateID> auto dead_out,
        std::output_iterator<StateID> auto non_proper_out,
        std::output_iterator<StateID> auto proper_out,
        utils::CountdownTimer& timer);
};

} // namespace preprocessing
} // namespace probfd

#define GUARD_INCLUDE_PROBFD_PREPROCESSING_QUALITATIVE_REACHABILITY_ANALYSIS_H
#include "probfd/preprocessing/qualitative_reachability_analysis_impl.h"
#undef GUARD_INCLUDE_PROBFD_PREPROCESSING_QUALITATIVE_REACHABILITY_ANALYSIS_H

#endif // __END_COMPONENT_DECOMPOSITION_H__