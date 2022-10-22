#ifndef MDPS_END_COMPONENT_DECOMPOSITION_END_COMPONENT_DECOMPOSITION_H
#define MDPS_END_COMPONENT_DECOMPOSITION_END_COMPONENT_DECOMPOSITION_H

#include "utils/iterators.h"
#include "utils/timer.h"

#include "probfd/engine_interfaces/action_id_map.h"
#include "probfd/engine_interfaces/reward_function.h"
#include "probfd/engine_interfaces/state_evaluator.h"
#include "probfd/engine_interfaces/state_id_map.h"
#include "probfd/engine_interfaces/transition_generator.h"

#include "probfd/heuristics/pdbs/qualitative_result_store.h"

#include "probfd/quotient_system/engine_interfaces.h"
#include "probfd/quotient_system/quotient_system.h"

#include "probfd/storage/per_state_storage.h"

#include <cassert>
#include <deque>
#include <iostream>
#include <iterator>
#include <memory>
#include <type_traits>
#include <vector>

namespace probfd {

/// Namespace dedicated to end component decomposition.
namespace end_components {

/**
 * @brief Contains printable statistics for the end component decomposition.
 *
 */
struct Statistics {
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
 * An end component is a sub-MDP \f$M = (S, E)\f$ where \f$S\f$ is a subset
 * of states and \f$E : S \to 2^A \f$ is a mapping from states to (enabled)
 * actions. \f$M\f$ must satisfy:
 * 1. The directed graph induced by \f$M\f$ is strongly connected.
 * 2. \f$E(s) \subseteq A(s)\f$ for every state \f$s \in S\f$.
 * 3. \f$M\f$ is closed under probabilistic branching. If \f$s \in S\f$, \f$a
 * \in E(s)\f$ and \f$\Pr(t \mid s, a) > 0\f$, then \f$t \in S\f$.
 *
 * An end component is trivial if it contains a single state \f$S = \{s\}\f$
 * and \f$E(s) = \emptyset\f$. Furthermore, a maximal end component (MEC) is an
 * end component that is maximal with respect to set inclusion.
 *
 * This algorithm computes the maximal end components of an MDP and builds the
 * MDP quotient in which every maximal end component \f$M\f$ is collapsed into
 * a single state.
 *
 * In MaxProb analysis, the existence of non-trivial end components prevents
 * algorithms based on value iteration from converging against the optimal value
 * function when started with a non-admissible value function.
 * However, the end component decomposition quotient has only trivial end
 * components, therefore standard value iteration algorithms converge from any
 * initial value function. Every state in a maximal end component has the same
 * MaxProb value, which makes it easy to extract the optimal value function.
 *
 * @see engines::interval_iteration::IntervalIteration
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
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
        explicit ExpansionInfo(
            unsigned stck,
            std::vector<Action> aops,
            std::vector<std::vector<StateID>> successors)
            : stck(stck)
            , lstck(stck)
            , aops(std::move(aops))
            , successors(std::move(successors))
        {
        }

        bool next_action()
        {
            assert(aops.size() == successors.size());
            aops.pop_back();
            successors.pop_back();
            return !aops.empty();
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

        // whether the current transition remains in the scc
        bool remains_in_scc = true;

        // recursive decomposition flag
        // Recurse if there is a transition that can leave and remain in the scc
        // If a always remains in the SCC, then a does not violate the EC
        // condition If a always goes out of the SCC, then no edge of a was part
        // of it
        bool recurse = false;

        std::vector<Action> aops;
        std::vector<std::vector<StateID>> successors;
    };

    struct StackInfo {
        explicit StackInfo(const StateID& sid)
            : stateid(sid)
            , successors(1)
        {
        }

        StateID stateid;

        // SCC successors for ECD recursion
        std::vector<Action> aops;
        std::vector<std::vector<StateID>> successors;
    };

public:
    using QuotientSystem = quotient_system::QuotientSystem<Action>;

    EndComponentDecomposition(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<Action>* action_id_map,
        engine_interfaces::RewardFunction<State, Action>* rewards,
        engine_interfaces::TransitionGenerator<Action>* transition_gen,
        bool expand_goals,
        const engine_interfaces::StateEvaluator<State>* pruning_function =
            nullptr)
        : state_id_map_(state_id_map)
        , rewards_(rewards)
        , transition_gen_(transition_gen)
        , expand_goals_(expand_goals)
        , pruning_function_(pruning_function)
        , sys_(new QuotientSystem(action_id_map, transition_gen_))
    {
    }

    /**
     * @brief Build the quotient of the MDP with respect to the maximal end
     * components.
     *
     * Only the fragment of the MDP that is reachable from the given initial
     * state is considered.
     */
    std::unique_ptr<QuotientSystem>
    build_quotient_system(const State& initial_state)
    {
        stats_ = Statistics();

        auto init_id = state_id_map_->get_state_id(initial_state);
        push<true>(init_id, state_infos_[init_id]);

        find_and_decompose_sccs<true>();

        assert(stack_.empty());
        assert(expansion_queue_.empty());
        stats_.time.stop();

        return std::move(sys_);
    }

    void print_statistics(std::ostream& out) const { stats_.print(out); }

    Statistics get_statistics() const { return stats_; }

private:
    template <bool RootIteration>
    bool push(StateID state_id, StateInfo& state_info);

    template <>
    bool push<true>(StateID state_id, StateInfo& state_info)
    {
        state_info.explored = 1;
        State state = state_id_map_->get_state(state_id);

        if (rewards_->operator()(state)) {
            ++stats_.terminals;
            ++stats_.goals;

            if (!expand_goals_) {
                return false;
            }

            state_info.expandable_goal = 1;
        } else if (
            pruning_function_ != nullptr &&
            pruning_function_->operator()(state)) {
            ++stats_.terminals;
            return false;
        }

        std::vector<Action> aops;
        transition_gen_->operator()(state_id, aops);

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
            transition_gen_->operator()(state_id, aops[i], transition);

            std::vector<StateID> succ_ids;

            for (const StateID& succ_id : transition.elements()) {
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
            std::move(successors));

        state_info.stackid_ = stack_.size();
        stack_.emplace_back(state_id);

        return true;
    }

    template <>
    bool push<false>(StateID state_id, StateInfo& info)
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
    void find_and_decompose_sccs(const unsigned limit = 0)
    {
        if (expansion_queue_.size() <= limit) {
            return;
        }

        ExpansionInfo* e = &expansion_queue_.back();
        StackInfo* s = &stack_[e->stck];

        for (;;) {
            // DFS recursion
            while (push_successor<RootIteration>(*e, *s)) {
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
                    scc_found<RootIteration>(*e, *s);
                }

                expansion_queue_.pop_back();

                if (expansion_queue_.size() <= limit) {
                    goto break_exploration;
                }

                e = &expansion_queue_.back();
                s = &stack_[e->stck];

                const auto action_reward =
                    (*rewards_)(s->stateid, e->get_current_action());

                if (is_onstack) {
                    e->lstck = std::min(e->lstck, lstck);

                    s->successors.back().emplace_back(
                        e->get_current_successor());
                    e->recurse = e->recurse || recurse || !e->remains_in_scc ||
                                 action_reward != value_type::zero;
                } else {
                    e->recurse = e->recurse || !s->successors.back().empty();
                    e->remains_in_scc = false;
                }

                if (e->next_successor()) {
                    break;
                }

                if (e->remains_in_scc && action_reward == value_type::zero) {
                    assert(!s->successors.back().empty());
                    s->aops.push_back(e->get_current_action());
                    s->successors.emplace_back();
                } else {
                    e->remains_in_scc = true;
                    s->successors.back().clear();
                }

                if (e->next_action()) {
                    break;
                }

                assert(s->successors.back().empty());
                s->successors.pop_back();
            }
        }

    break_exploration:;
    }

    template <bool RootIteration>
    bool push_successor(ExpansionInfo& e, StackInfo& s)
    {
        do {
            std::vector<StateID>& s_succs = s.successors.back();

            const auto action_reward = (*rewards_)(s.stateid, e.aops.back());

            do {
                const StateID succ_id = e.get_current_successor();
                StateInfo& succ_info = state_infos_[succ_id];

                // NOTE: Exploration status must be checked first.
                // During recursive decomposition, the exploration status is
                // reset but the stack index is maintained for the time being.
                // The node is however not logically on the stack.
                // Therefore the onstack check cannot be moved up, we have to
                // resort to goto to avoid code duplication.
                if (!succ_info.explored) {
                    if (push<RootIteration>(succ_id, succ_info)) {
                        return true;
                    }

                    goto backtrack_child_scc;
                } else if (succ_info.onstack()) {
                    e.lstck = std::min(e.lstck, succ_info.stackid_);

                    s_succs.emplace_back(succ_id);
                    e.recurse = e.recurse || !e.remains_in_scc ||
                                action_reward != value_type::zero;
                } else {
                backtrack_child_scc:
                    e.recurse = e.recurse || !s_succs.empty();
                    e.remains_in_scc = false;
                }
            } while (e.next_successor());

            if (e.remains_in_scc && action_reward == value_type::zero) {
                assert(!s_succs.empty());
                s.successors.emplace_back();
                s.aops.push_back(e.get_current_action());
            } else {
                s_succs.clear();
                e.remains_in_scc = true;
            }
        } while (e.next_action());

        assert(s.successors.back().empty());
        s.successors.pop_back();

        return false;
    }

    template <bool RootIteration>
    void scc_found(ExpansionInfo& e, StackInfo& s)
    {
        const unsigned scc_size = stack_.size() - e.stck;
        auto scc_begin = stack_.begin() + e.stck;
        auto scc_end = stack_.end();

        StateID scc_repr_id = s.stateid;
        if (scc_size == 1) {
            assert(s.aops.empty());
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
                for (auto it = scc_begin; it != scc_end; ++it) {
                    assert(it->successors.size() == it->aops.size());
                    StateInfo& info = state_infos_[it->stateid];
                    if (info.expandable_goal) {
                        it->successors.clear();
                        it->aops.clear();
                        e.recurse = true;
                    }
                }
            }

            if (e.recurse) {
                ++stats_.recursions;

                if constexpr (RootIteration) {
                    ++stats_.sccsk;
                }

                for (auto it = scc_begin; it != scc_end; ++it) {
                    assert(it->successors.size() == it->aops.size());
                    state_infos_[it->stateid].explored = 0;
                }

                decompose(e.stck);
            } else {
                unsigned transitions = 0;

                for (auto it = scc_begin; it != scc_end; ++it) {
                    assert(it->successors.size() == it->aops.size());
                    StateInfo& info = state_infos_[it->stateid];
                    info.stackid_ = StateInfo::UNDEF;

                    transitions += it->aops.size();
                }

                auto begin = utils::make_transform_iterator(
                    scc_begin,
                    &StackInfo::stateid);
                auto end = utils::make_transform_iterator(
                    scc_end,
                    &StackInfo::stateid);
                auto abegin =
                    utils::make_transform_iterator(scc_begin, &StackInfo::aops);
                sys_->build_quotient(begin, end, scc_repr_id, abegin);
                stack_.erase(scc_begin, scc_end);

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

    using StackIterator = typename std::vector<StackInfo>::iterator;

    void decompose(unsigned start)
    {
        const unsigned limit = expansion_queue_.size();

        for (unsigned i = start; i < stack_.size(); ++i) {
            StackInfo& stack_info = stack_[i];
            const StateID id = stack_info.stateid;
            StateInfo& state_info = state_infos_[id];

            if (state_info.explored) {
                continue;
            }

            push<false>(id, state_info);

            // Recursively run decomposition
            find_and_decompose_sccs<false>(limit);
        }

        stack_.erase(stack_.begin() + start, stack_.end());

        assert(expansion_queue_.size() == limit);
    }

    engine_interfaces::StateIDMap<State>* state_id_map_;
    engine_interfaces::RewardFunction<State, Action>* rewards_;
    engine_interfaces::TransitionGenerator<Action>* transition_gen_;

    bool expand_goals_;

    const engine_interfaces::StateEvaluator<State>* pruning_function_;

    std::unique_ptr<QuotientSystem> sys_;

    storage::PerStateStorage<StateInfo> state_infos_;
    std::deque<ExpansionInfo> expansion_queue_;
    std::vector<StackInfo> stack_;

    Statistics stats_;
};

} // namespace end_components
} // namespace probfd

#endif // __END_COMPONENT_DECOMPOSITION_H__
