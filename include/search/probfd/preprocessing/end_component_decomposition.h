#ifndef PROBFD_PREPROCESSING_END_COMPONENT_DECOMPOSITION_H
#define PROBFD_PREPROCESSING_END_COMPONENT_DECOMPOSITION_H

#include "probfd/quotients/quotient_system.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/utils/iterators.h"

#include "probfd/evaluator.h"
#include "probfd/mdp.h"
#include "probfd/type_traits.h"

#include "downward/utils/countdown_timer.h"

#include <cassert>
#include <deque>
#include <iostream>
#include <iterator>
#include <limits>
#include <memory>
#include <ranges>
#include <type_traits>
#include <vector>

namespace probfd {

/// This namespace contains preprocessing algorithms for SSPs.
namespace preprocessing {

/**
 * @brief Contains printable statistics for the end component decomposition.
 */
struct ECDStatistics {
    void print(std::ostream& out) const
    {
        out << "  Terminal states: " << terminals << " (" << goals
            << " goal states, " << selfloops << " self loop states)"
            << std::endl;
        out << "  Singleton SCC(s): " << sccs1 << " (" << sccs1_dead << " dead)"
            << std::endl;
        out << "  Non-singleton SCC(s): " << sccsk << " (" << sccsk_dead
            << " dead)" << std::endl;
        out << "  Singleton end component(s): " << ec1 << std::endl;
        out << "  Non-singleton end component(s): " << eck << std::endl;
        out << "  End-component transitions: " << ec_transitions << std::endl;
        out << "  Recursive calls: " << recursions << std::endl;
        out << "  End component computation: " << time << std::endl;
    }

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
    struct StateInfo {
        static const unsigned UNDEF = std::numeric_limits<unsigned>::max() >> 2;

        explicit StateInfo()
            : explored(0)
            , expandable_goal(0)
            , stackid_(UNDEF)
        {
        }

        bool onstack() const { return stackid_ < UNDEF; }

        unsigned explored : 1;
        unsigned expandable_goal : 1; // non-terminal goal?
        unsigned stackid_ : 30;
    };

    struct ExpansionInfo {
        // Used in decomposition recursion
        ExpansionInfo(
            unsigned stck,
            std::vector<Action> aops,
            std::vector<std::vector<StateID>> successors)
            : stck(stck)
            , lstck(stck)
            , nz_or_leaves_scc(false)
            , aops(std::move(aops))
            , successors(std::move(successors))
        {
        }

        // Used in root iteration
        ExpansionInfo(
            unsigned stck,
            std::vector<Action> aops,
            std::vector<std::vector<StateID>> successors,
            MDP<State, Action>& mdp)
            : stck(stck)
            , lstck(stck)
            , nz_or_leaves_scc(mdp.get_action_cost(aops.back()) != 0_vt)
            , aops(std::move(aops))
            , successors(std::move(successors))
        {
        }

        bool next_action()
        {
            assert(aops.size() == successors.size());
            aops.pop_back();
            successors.pop_back();
            nz_or_leaves_scc = false;
            return !aops.empty();
        }

        bool next_action(MDP<State, Action>& mdp)
        {
            assert(aops.size() == successors.size());
            aops.pop_back();
            successors.pop_back();

            if (!aops.empty()) {
                nz_or_leaves_scc = mdp.get_action_cost(aops.back()) != 0_vt;
                return true;
            }

            return false;
        }

        bool next_successor()
        {
            auto& succs = successors.back();
            succs.pop_back();
            return !succs.empty();
        }

        StateID get_current_successor() { return successors.back().back(); }

        Action& get_current_action() { return aops.back(); }

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
    };

    struct StackInfo {
        StateID stateid;

        // SCC successors for ECD recursion
        std::vector<Action> aops;
        std::vector<std::vector<StateID>> successors;

        explicit StackInfo(StateID sid)
            : stateid(sid)
            , successors(1)
        {
        }

        template <size_t i>
        friend auto& get(StackInfo& info)
        {
            if constexpr (i == 0) return info.stateid;
            if constexpr (i == 1) return info.aops;
        }

        template <size_t i>
        friend const auto& get(const StackInfo& info)
        {
            if constexpr (i == 0) return info.stateid;
            if constexpr (i == 1) return info.aops;
        }
    };

    const bool expand_goals_;
    const Evaluator<State>* pruning_function_;

    storage::PerStateStorage<StateInfo> state_infos_;
    std::deque<ExpansionInfo> expansion_queue_;
    std::vector<StackInfo> stack_;

    ECDStatistics stats_;

public:
    using QuotientSystem = quotients::QuotientSystem<State, Action>;

    EndComponentDecomposition(
        bool expand_goals,
        const Evaluator<State>* pruning_function = nullptr)
        : expand_goals_(expand_goals)
        , pruning_function_(pruning_function)
    {
    }

    /**
     * @brief Build the quotient of the MDP with respect to the maximal end
     * components.
     *
     * Only the fragment of the MDP that is reachable from the given initial
     * state is considered.
     */
    std::unique_ptr<QuotientSystem> build_quotient_system(
        MDP<State, Action>& mdp,
        param_type<State> initial_state,
        double max_time = std::numeric_limits<double>::infinity())
    {
        utils::CountdownTimer timer(max_time);

        stats_ = ECDStatistics();
        auto sys = std::make_unique<QuotientSystem>(&mdp);

        auto init_id = mdp.get_state_id(initial_state);

        if (push_root(mdp, init_id, state_infos_[init_id])) {
            find_and_decompose_sccs<true>(mdp, *sys, 0, timer);
        }

        assert(stack_.empty());
        assert(expansion_queue_.empty());
        stats_.time.stop();

        return sys;
    }

    void print_statistics(std::ostream& out) const { stats_.print(out); }

    ECDStatistics get_statistics() const { return stats_; }

private:
    bool
    push_root(MDP<State, Action>& mdp, StateID state_id, StateInfo& state_info)
    {
        state_info.explored = 1;
        State state = mdp.get_state(state_id);

        if (mdp.get_termination_info(state).is_goal_state()) {
            ++stats_.terminals;
            ++stats_.goals;

            if (!expand_goals_) {
                return false;
            }

            state_info.expandable_goal = 1;
        } else if (
            pruning_function_ != nullptr &&
            pruning_function_->evaluate(state).is_unsolvable()) {
            ++stats_.terminals;
            return false;
        }

        std::vector<Action> aops;
        mdp.generate_applicable_actions(state_id, aops);

        if (aops.empty()) {
            if (expand_goals_ && state_info.expandable_goal) {
                state_info.expandable_goal = 0;
            } else {
                ++stats_.terminals;
            }

            return false;
        }

        std::vector<std::vector<StateID>> successors;
        successors.reserve(aops.size());

        unsigned non_loop_actions = 0;
        for (unsigned i = 0; i < aops.size(); ++i) {
            Distribution<StateID> transition;
            mdp.generate_action_transitions(state_id, aops[i], transition);

            std::vector<StateID> succ_ids;

            for (StateID succ_id : transition.support()) {
                if (succ_id != state_id) {
                    succ_ids.push_back(succ_id);
                }
            }

            if (!succ_ids.empty()) {
                successors.emplace_back(std::move(succ_ids));

                if (i != non_loop_actions) {
                    aops[non_loop_actions] = aops[i];
                }

                ++non_loop_actions;
            }
        }

        // only self-loops
        if (non_loop_actions == 0) {
            if (expand_goals_ && state_info.expandable_goal) {
                state_info.expandable_goal = 0;
            } else {
                ++stats_.terminals;
                ++stats_.selfloops;
            }

            return false;
        }

        aops.erase(aops.begin() + non_loop_actions, aops.end());

        expansion_queue_.emplace_back(
            stack_.size(),
            std::move(aops),
            std::move(successors),
            mdp);

        state_info.stackid_ = stack_.size();
        stack_.emplace_back(state_id);

        return true;
    }

    bool push_ecd(StateID state_id, StateInfo& info)
    {
        assert(!info.explored);
        assert(info.onstack());

        info.explored = true;
        StackInfo& scc_info = stack_[info.stackid_];

        if (scc_info.successors.empty()) {
            info.stackid_ = StateInfo::UNDEF;
            ++stats_.ec1;
            return false;
        }

        const auto stack_size = stack_.size();
        info.stackid_ = stack_size;

        expansion_queue_.emplace_back(
            stack_size,
            std::move(scc_info.aops),
            std::move(scc_info.successors));

        stack_.emplace_back(state_id);

        return true;
    }

    template <bool RootIteration>
    void find_and_decompose_sccs(
        MDP<State, Action>& mdp,
        QuotientSystem& sys,
        const unsigned limit,
        utils::CountdownTimer& timer)
    {
        if (expansion_queue_.size() <= limit) {
            return;
        }

        ExpansionInfo* e = &expansion_queue_.back();
        StackInfo* s = &stack_[e->stck];

        for (;;) {
            // DFS recursion
            while (push_successor<RootIteration>(mdp, *e, *s, timer)) {
                e = &expansion_queue_.back();
                s = &stack_[e->stck];
            }

            // Iterative backtracking
            for (;;) {
                assert(e->successors.empty() && e->aops.empty());

                const bool recurse = e->recurse;
                const unsigned int stck = e->stck;
                const unsigned int lstck = e->lstck;

                assert(stck >= lstck);

                const bool is_onstack = stck != lstck;

                if (!is_onstack) {
                    scc_found<RootIteration>(mdp, sys, *e, *s, timer);
                }

                expansion_queue_.pop_back();

                if (expansion_queue_.size() <= limit) {
                    return;
                }

                timer.throw_if_expired();

                e = &expansion_queue_.back();
                s = &stack_[e->stck];

                auto& stk_successors = s->successors.back();

                if (is_onstack) {
                    e->lstck = std::min(e->lstck, lstck);

                    stk_successors.emplace_back(e->get_current_successor());
                    e->recurse = e->recurse || recurse || e->nz_or_leaves_scc;
                } else {
                    e->recurse = e->recurse || !stk_successors.empty();
                    e->nz_or_leaves_scc = true;
                }

                if (e->next_successor()) {
                    break;
                }

                if (!e->nz_or_leaves_scc) {
                    assert(!stk_successors.empty());
                    s->aops.push_back(e->get_current_action());
                    s->successors.emplace_back();
                } else {
                    stk_successors.clear();
                }

                bool next_action;

                if constexpr (RootIteration) {
                    next_action = e->next_action(mdp);
                } else {
                    next_action = e->next_action();
                }

                if (next_action) {
                    break;
                }

                assert(s->successors.back().empty());
                s->successors.pop_back();
            }
        }
    }

    template <bool RootIteration>
    bool push_successor(
        MDP<State, Action>& mdp,
        ExpansionInfo& e,
        StackInfo& s,
        utils::CountdownTimer& timer)
    {
        for (;;) {
            std::vector<StateID>& s_succs = s.successors.back();

            do {
                timer.throw_if_expired();

                const StateID succ_id = e.get_current_successor();
                StateInfo& succ_info = state_infos_[succ_id];

                // NOTE: Exploration status must be checked first.
                // During recursive decomposition, the exploration status is
                // reset but the stack index is maintained for the time being.
                // The node is however not logically on the stack.
                // Therefore the onstack check cannot be moved up, we have to
                // resort to goto to avoid code duplication.
                if (!succ_info.explored) {
                    if constexpr (RootIteration) {
                        if (push_root(mdp, succ_id, succ_info)) return true;
                    } else {
                        if (push_ecd(succ_id, succ_info)) return true;
                    }

                    goto backtrack_child_scc;
                } else if (succ_info.onstack()) {
                    e.lstck = std::min(e.lstck, succ_info.stackid_);

                    s_succs.emplace_back(succ_id);
                    e.recurse = e.recurse || e.nz_or_leaves_scc;
                } else {
                backtrack_child_scc:
                    e.recurse = e.recurse || !s_succs.empty();
                    e.nz_or_leaves_scc = true;
                }
            } while (e.next_successor());

            if (!e.nz_or_leaves_scc) {
                assert(!s_succs.empty());
                s.successors.emplace_back();
                s.aops.push_back(e.get_current_action());
            } else {
                s_succs.clear();
            }

            bool next_action;

            if constexpr (RootIteration) {
                next_action = e.next_action(mdp);
            } else {
                next_action = e.next_action();
            }

            if (!next_action) {
                break;
            }
        }

        assert(s.successors.back().empty());
        s.successors.pop_back();

        return false;
    }

    template <bool RootIteration>
    void scc_found(
        MDP<State, Action>& mdp,
        QuotientSystem& sys,
        ExpansionInfo& e,
        StackInfo& s,
        utils::CountdownTimer& timer)
    {
        auto scc = stack_ | std::views::drop(e.stck);

        if (scc.size()) {
            assert(s.aops.empty());
            const StateID scc_repr_id = s.stateid;
            StateInfo& info = state_infos_[scc_repr_id];
            info.stackid_ = StateInfo::UNDEF;

            stack_.pop_back();

            // Update stats
            ++stats_.ec1;

            if constexpr (RootIteration) {
                ++stats_.sccs1;
            }
        } else {
            if (expand_goals_) {
                for (auto& stk_info : scc) {
                    assert(stk_info.successors.size() == stk_info.aops.size());
                    StateInfo& info = state_infos_[stk_info.stateid];
                    if (info.expandable_goal) {
                        stk_info.successors.clear();
                        stk_info.aops.clear();
                        e.recurse = true;
                    }
                }
            }

            if (e.recurse) {
                ++stats_.recursions;

                if constexpr (RootIteration) {
                    ++stats_.sccsk;
                }

                for (const auto& stk_info : scc) {
                    assert(stk_info.successors.size() == stk_info.aops.size());
                    state_infos_[stk_info.stateid].explored = 0;
                }

                decompose(mdp, sys, e.stck, timer);
            } else {
                unsigned transitions = 0;

                for (const auto& stk_info : scc) {
                    assert(stk_info.successors.size() == stk_info.aops.size());
                    StateInfo& info = state_infos_[stk_info.stateid];
                    info.stackid_ = StateInfo::UNDEF;

                    transitions += stk_info.aops.size();
                }

                sys.build_new_quotient(scc, s);
                stack_.erase(scc.begin(), scc.end());

                // Update stats
                ++stats_.eck;
                stats_.ec_transitions += transitions;

                if constexpr (RootIteration) {
                    ++stats_.sccsk;
                }
            }
        }

        assert(stack_.size() == e.stck);
    }

    void decompose(
        MDP<State, Action>& mdp,
        QuotientSystem& sys,
        unsigned start,
        utils::CountdownTimer& timer)
    {
        const unsigned limit = expansion_queue_.size();

        for (unsigned i = start; i < stack_.size(); ++i) {
            const StateID id = stack_[i].stateid;
            StateInfo& state_info = state_infos_[id];

            if (!state_info.explored && push_ecd(id, state_info)) {
                // Recursively run decomposition
                find_and_decompose_sccs<false>(mdp, sys, limit, timer);
            }
        }

        stack_.erase(stack_.begin() + start, stack_.end());

        assert(expansion_queue_.size() == limit);
    }
};

} // namespace preprocessing
} // namespace probfd

#endif // __END_COMPONENT_DECOMPOSITION_H__
