#ifndef PROBFD_PREPROCESSING_END_COMPONENT_DECOMPOSITION_H
#define PROBFD_PREPROCESSING_END_COMPONENT_DECOMPOSITION_H

#include "probfd/quotients/quotient_system.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/evaluator.h"
#include "probfd/mdp.h"
#include "probfd/type_traits.h"

#include <deque>
#include <limits>
#include <memory>
#include <ostream>
#include <vector>

namespace utils {
class CountdownTimer;
}

namespace probfd {

/// This namespace contains preprocessing algorithms for SSPs.
namespace preprocessing {

/**
 * @brief Contains printable statistics for the end component decomposition.
 */
struct ECDStatistics {
    unsigned long long goals = 0;
    unsigned long long terminals = 0;
    unsigned long long selfloops = 0;

    unsigned long long sccs1 = 0;
    unsigned long long sccsk = 0;
    unsigned long long sccs1_dead = 0;
    unsigned long long sccsk_dead = 0;

    unsigned long long ec1 = 0;
    unsigned long long eck = 0;

    unsigned long long ec_transitions = 0;

    unsigned long long recursions = 0;

    utils::Timer time;

    void print(std::ostream& out) const;
};

/**
 * @brief A builder class that implements end component decomposition.
 *
 * An (non-trivial) end component is a sub-MDP \f$M = (S, E)\f$ where \f$S\f$
 * is a subset of states and \f$E : S \to 2^A \f$ is a mapping from states to
 * a non-empty set of enabled actions. \f$M\f$ must satisfy:
 * 1. The directed graph induced by \f$M\f$ is strongly connected.
 * 2. \f$E(s) \subseteq A(s)\f$ for every state \f$s \in S\f$.
 * 3. \f$M\f$ is closed under probabilistic branching. If \f$s \in S\f$, \f$a
 * \in E(s)\f$ and \f$\Pr(t \mid s, a) > 0\f$, then \f$t \in S\f$.
 *
 * A <em>zero EC</em> is an end component which only contains actions that have
 * a cost of zero. Furthermore, a <em>maximal end component</em> (MEC) is an
 * end component that is maximal with respect to set inclusion.
 *
 * This algorithm computes all maximal zero ECs of the MDP in time O(m*n),
 * where $m$ is the number of edges and $n$ is the number of nodes in the
 * induced graph of the MDP. The returned quotient collapses all zero ECs
 * into a single state.
 *
 * The existence of zero ECs prevents ordinary algorithms based on value
 * iteration from converging against the optimal value function, unless a
 * pessimistic heuristic is used to initialize the value function. However, the
 * end component decomposition quotient has no zero ECs, therefore standard
 * value iteration algorithms converge with any initialization of the value
 * function. Since every state in a zero EC has the same optimal state value,
 * it easy to extendt the final potimal value function for the quotient to
 * the optimal value function for the original MDP.
 *
 * @see engines::interval_iteration::IntervalIteration
 *
 * @tparam State - The state type of the underlying state space.
 * @tparam Action - The action type of the underlying state space.
 */
template <typename State, typename Action>
class EndComponentDecomposition {
    using MDP = MDP<State, Action>;
    using Evaluator = Evaluator<State>;
    using QuotientSystem = quotients::QuotientSystem<State, Action>;

    struct StateInfo {
        static constexpr uint32_t UNDEF =
            std::numeric_limits<uint32_t>::max() >> 2;

        unsigned explored : 1 = 0;
        unsigned expandable_goal : 1 = 0; // non-terminal goal?
        unsigned stackid_ : 30 = UNDEF;

        bool onstack() const;
    };

    struct ExpansionInfo {
        // Tarjan's SCC algorithm: stack id and lowlink
        const unsigned stck;
        unsigned lstck;

        // whether the transition has non-zero cost or can leave the scc
        bool nz_or_leaves_scc;

        // recursive decomposition flag
        // Recursively decompose the SCC if there is a zero-cost transition
        // in it that can leave and remain in the scc.
        bool recurse = false;

        std::vector<Action> aops;
        std::vector<std::vector<StateID>> successors;

        // Used in decomposition recursion
        ExpansionInfo(
            unsigned stck,
            std::vector<Action> aops,
            std::vector<std::vector<StateID>> successors);

        // Used in root iteration
        ExpansionInfo(
            unsigned stck,
            std::vector<Action> aops,
            std::vector<std::vector<StateID>> successors,
            MDP& mdp);

        // Used in decomposition recursion
        bool next_action();

        // Used in root iteration
        bool next_action(MDP& mdp);

        bool next_successor();

        StateID get_current_successor();
        Action& get_current_action();
    };

    struct StackInfo;

    const bool expand_goals_;

    storage::PerStateStorage<StateInfo> state_infos_;
    std::deque<ExpansionInfo> expansion_queue_;
    std::vector<StackInfo> stack_;

    ECDStatistics stats_;

public:
    explicit EndComponentDecomposition(bool expand_goals);

    /**
     * @brief Build the quotient of the MDP with respect to the maximal end
     * components.
     *
     * Only the fragment of the MDP that is reachable from the given initial
     * state is considered.
     */
    std::unique_ptr<QuotientSystem> build_quotient_system(
        MDP& mdp,
        const Evaluator* pruning_function,
        param_type<State> initial_state,
        double max_time = std::numeric_limits<double>::infinity());

    void print_statistics(std::ostream& out) const;

    ECDStatistics get_statistics() const;

private:
    // Used in root iteration
    bool push(
        StateID state_id,
        StateInfo& state_info,
        MDP& mdp,
        const Evaluator* pruning_function);

    // Used in decomposition recursion
    bool push(StateID state_id, StateInfo& info);

    void find_and_decompose_sccs(
        QuotientSystem& sys,
        const unsigned limit,
        utils::CountdownTimer& timer,
        auto&... mdp_and_h);

    bool push_successor(
        ExpansionInfo& e,
        StackInfo& s,
        utils::CountdownTimer& timer,
        auto&... mdp_and_h);

    template <bool RootIteration>
    void scc_found(
        QuotientSystem& sys,
        ExpansionInfo& e,
        StackInfo& s,
        utils::CountdownTimer& timer);

    void decompose(
        QuotientSystem& sys,
        unsigned start,
        utils::CountdownTimer& timer);
};

} // namespace preprocessing
} // namespace probfd

#define GUARD_INCLUDE_PROBFD_PREPROCESSING_END_COMPONENT_DECOMPOSITION_H
#include "probfd/preprocessing/end_component_decomposition_impl.h"
#undef GUARD_INCLUDE_PROBFD_PREPROCESSING_END_COMPONENT_DECOMPOSITION_H

#endif // __END_COMPONENT_DECOMPOSITION_H__
