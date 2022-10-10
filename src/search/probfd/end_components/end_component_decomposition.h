#ifndef MDPS_END_COMPONENT_DECOMPOSITION_END_COMPONENT_DECOMPOSITION_H
#define MDPS_END_COMPONENT_DECOMPOSITION_END_COMPONENT_DECOMPOSITION_H

#include "../../utils/iterators.h"
#include "../../utils/timer.h"
#include "../engine_interfaces/action_id_map.h"
#include "../engine_interfaces/reward_function.h"
#include "../engine_interfaces/state_evaluator.h"
#include "../engine_interfaces/state_id_map.h"
#include "../engine_interfaces/transition_generator.h"
#include "../heuristics/pdbs/qualitative_result_store.h"
#include "../quotient_system/engine_interfaces.h"
#include "../quotient_system/quotient_system.h"
#include "../storage/per_state_storage.h"

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

namespace internal {

struct StateInfo {
    static const unsigned UNDEF = (((unsigned)-1) >> 2) - 2;
    static const unsigned ZERO = UNDEF + 1;
    static const unsigned ONE = UNDEF + 2;

    explicit StateInfo()
        : explored(0)
        , expandable_goal(0)
        , stackid_(UNDEF)
    {
    }

    unsigned stackid() const
    {
        assert(onstack());
        return stackid_;
    }
    bool onstack() const { return stackid_ < UNDEF; }

    unsigned explored : 1;
    unsigned expandable_goal : 1; // non-terminal goal?
    unsigned stackid_ : 30;
};

template <typename Action>
struct ExpansionInfo {
    explicit ExpansionInfo(unsigned stck)
        : stck(stck)
        , lstck(stck)
    {
    }

    // Tarjan's SCC algorithm: stack id and lowlink
    const unsigned stck;
    unsigned lstck;

    // whether the current transition remains in the scc
    bool remains_in_scc = true;

    // recursive decomposition flag
    // Recurse if there is a transition that can leave and remain in the scc
    // If a always remains in the SCC, then a does not violate the EC condition
    // If a always goes out of the SCC, then no edge of a was part of it
    bool recurse = false;

    std::vector<Action> aops;
    std::vector<std::vector<StateID>> successors;
};

template <typename Action>
struct StackInfo {
    explicit StackInfo(const StateID& sid)
        : stateid(sid)
        , successors(1)
    {
    }

    StateID stateid;
    std::vector<Action> aops;
    std::vector<std::vector<StateID>> successors;
};

} // namespace internal

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
    using StateInfo = internal::StateInfo;
    using ExpansionInfo = internal::ExpansionInfo<Action>;
    using StackInfo = internal::StackInfo<Action>;

    using StateInfoStore = storage::PerStateStorage<StateInfo>;
    using ExpansionQueue = std::deque<ExpansionInfo>;
    using Stack = std::vector<StackInfo>;

public:
    using QuotientSystem = quotient_system::QuotientSystem<Action>;

    EndComponentDecomposition(
        StateIDMap<State>* state_id_map,
        ActionIDMap<Action>* action_id_map,
        RewardFunction<State, Action>* rewards,
        TransitionGenerator<Action>* transition_gen,
        bool expand_goals,
        const StateEvaluator<State>* pruning_function = nullptr)
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
        push(init_id, state_infos_[init_id]);

        auto get_succ_id = [](const StateID& id) { return id; };
        auto pushf = [this](const StateID& state_id, StateInfo& state_info) {
            return push(state_id, state_info);
        };

        find_and_decompose_sccs<true>(0, pushf, state_infos_, get_succ_id);

        assert(stack_.empty());
        assert(expansion_queue_.empty());
        stats_.time.stop();

        return std::move(sys_);
    }

    void print_statistics(std::ostream& out) const { stats_.print(out); }

    Statistics get_statistics() const { return stats_; }

private:
    bool push(const StateID& state_id, StateInfo& state_info)
    {
        state_info.explored = 1;
        State state = state_id_map_->get_state(state_id);

        if (rewards_->operator()(state)) {
            ++stats_.terminals;
            ++stats_.goals;
            state_info.stackid_ = StateInfo::ONE;

            if (!expand_goals_) {
                return false;
            }

            state_info.expandable_goal = 1;
        } else if (
            pruning_function_ != nullptr &&
            pruning_function_->operator()(state)) {
            ++stats_.terminals;
            state_info.stackid_ = StateInfo::ZERO;
            return false;
        }

        std::vector<Action> aops;
        transition_gen_->operator()(state_id, aops);

        if (aops.empty()) {
            if (expand_goals_ && state_info.expandable_goal) {
                state_info.expandable_goal = 0;
            } else {
                ++stats_.terminals;
                state_info.stackid_ = StateInfo::ZERO;
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
                state_info.stackid_ = StateInfo::ZERO;
            }

            return false;
        }

        aops.erase(aops.begin() + non_loop_actions, aops.end());

        ExpansionInfo& e = expansion_queue_.emplace_back(stack_.size());
        e.aops = std::move(aops);
        e.successors = std::move(successors);

        state_info.stackid_ = stack_.size();
        stack_.emplace_back(state_id);

        return true;
    }

    struct PushLocal {
        explicit PushLocal(
            std::vector<StackInfo>* scc,
            ExpansionQueue* e,
            Stack* stack,
            Statistics* stats)
            : scc_(scc)
            , expansion_queue_(e)
            , stack_(stack)
            , stats_(stats)
        {
        }

        bool operator()(unsigned i, StateInfo& info) const
        {
            assert(!info.explored);
            info.explored = true;
            StackInfo& scc_info = scc_->operator[](i);

            if (scc_info.successors.empty()) {
                ++stats_->ec1;
                return false;
            }

            info.stackid_ = stack_->size();
            ExpansionInfo& e = expansion_queue_->emplace_back(stack_->size());
            e.successors.swap(scc_info.successors);
            e.aops.swap(scc_info.aops);
            stack_->emplace_back(scc_info.stateid);

            return true;
        }

    private:
        std::vector<StackInfo>* scc_;
        ExpansionQueue* expansion_queue_;
        Stack* stack_;
        Statistics* stats_;
    };

    struct StateInfoLookup {
        explicit StateInfoLookup(std::vector<StackInfo>* s, StateInfoStore* i)
            : s_(s)
            , i_(i)
        {
        }

        StateInfo& operator[](unsigned x) const
        {
            const StateID& sid = s_->operator[](x).stateid;
            return i_->operator[](sid);
        }

        StateInfo& operator[](const StateID& stateid) const
        {
            return i_->operator[](stateid);
        }

        std::vector<StackInfo>* s_;
        StateInfoStore* i_;
    };

    template <
        bool RootIteration,
        typename Push,
        typename GetStateInfo,
        typename GetSuccID>
    void find_and_decompose_sccs(
        const unsigned limit,
        Push& pushf,
        GetStateInfo& get_state_info,
        GetSuccID& get_state_id)
    {
        if (expansion_queue_.size() <= limit) {
            return;
        }

        ExpansionInfo* e = &expansion_queue_.back();
        StackInfo* s = &stack_[e->stck];

        for (;;) {
            // DFS recursion
            while (
                push_successor(*e, *s, pushf, get_state_info, get_state_id)) {
                e = &expansion_queue_.back();
                s = &stack_[e->stck];
            }

            assert(e->successors.empty() && e->aops.empty());
            assert(e->stck >= e->lstck);
            assert(s->successors.back().empty());

            s->successors.pop_back();

            bool recurse = e->recurse;
            unsigned int lstck = e->lstck;
            bool is_onstack = e->stck != e->lstck;

            if (e->stck == e->lstck) {
                scc_found<RootIteration>(*e, *s, get_state_info);
            }

            expansion_queue_.pop_back();

            if (expansion_queue_.size() <= limit) {
                break;
            }

            e = &expansion_queue_.back();
            s = &stack_[e->stck];

            // We returned from a recursive DFS call. Update the parent.

            const auto action_reward =
                (*rewards_)(get_state_id(s->stateid), e->aops.back());

            if (is_onstack) {
                e->lstck = std::min(e->lstck, lstck);
                e->recurse = e->recurse || recurse || !e->remains_in_scc ||
                             action_reward != value_type::zero;
            } else {
                e->recurse = e->recurse || (e->remains_in_scc &&
                                            s->successors.back().size() > 1);
                e->remains_in_scc = false;
            }

            if (e->successors.back().empty()) {
                if (e->remains_in_scc && action_reward == value_type::zero) {
                    assert(!s->successors.back().empty());
                    s->aops.push_back(e->aops.back());
                    s->successors.emplace_back();
                } else {
                    e->remains_in_scc = true;
                    s->successors.back().clear();
                }

                e->aops.pop_back();
                e->successors.pop_back();
            }
        }
    }

    template <typename Push, typename GetStateInfo, typename GetSuccID>
    bool push_successor(
        ExpansionInfo& e,
        StackInfo& s,
        Push& pushf,
        GetStateInfo& get_state_info,
        GetSuccID& get_state_id)
    {
        while (!e.successors.empty()) {
            std::vector<StateID>& e_succs = e.successors.back();
            std::vector<StateID>& s_succs = s.successors.back();
            assert(!e_succs.empty());

            const auto action_reward =
                (*rewards_)(get_state_id(s.stateid), e.aops.back());

            do {
                const auto succ_id = get_state_id(e_succs.back());
                e_succs.pop_back();
                StateInfo& succ_info = get_state_info[succ_id];

                if (!succ_info.explored) {
                    s_succs.emplace_back(stack_.size());
                    if (pushf(succ_id, succ_info)) {
                        return true;
                    }

                    assert(!succ_info.onstack());
                    e.recurse =
                        e.recurse || (e.remains_in_scc && s_succs.size() > 1);
                    e.remains_in_scc = false;
                } else if (succ_info.onstack()) {
                    s_succs.emplace_back(succ_info.stackid());
                    e.lstck = std::min(e.lstck, succ_info.stackid());
                    e.recurse = e.recurse || !e.remains_in_scc ||
                                action_reward != value_type::zero;
                } else {
                    e.recurse =
                        e.recurse || (e.remains_in_scc && !s_succs.empty());
                    e.remains_in_scc = false;
                }
            } while (!e_succs.empty());

            assert(e_succs.empty());

            if (e.remains_in_scc && action_reward == value_type::zero) {
                assert(!s_succs.empty());
                s.successors.emplace_back();
                s.aops.push_back(e.aops.back());
            } else {
                s_succs.clear();
                e.remains_in_scc = true;
            }

            e.aops.pop_back();
            e.successors.pop_back();
        }

        return false;
    }

    template <bool RootIteration, typename GetStateInfo>
    void scc_found(ExpansionInfo& e, StackInfo& s, GetStateInfo& get_state_info)
    {
        unsigned scc_size = stack_.size() - e.stck;
        auto scc_begin = stack_.begin() + e.stck;
        auto scc_end = stack_.end();

        StateID scc_repr_id = s.stateid;
        if (scc_size == 1) {
            assert(s.aops.empty());
            StateInfo& info = get_state_info[scc_repr_id];
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
                    StateInfo& info = get_state_info[it->stateid];
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
                    StateInfo& info = get_state_info[it->stateid];
                    info.stackid_ = StateInfo::UNDEF;
                    info.explored = 0;
                }

                decompose(e.stck);
            } else {
                unsigned transitions = 0;

                for (auto it = scc_begin; it != scc_end; ++it) {
                    assert(it->successors.size() == it->aops.size());
                    StateInfo& info = get_state_info[it->stateid];
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

    void decompose(const unsigned start)
    {
        const unsigned scc_size = stack_.size() - start;
        auto scc_begin = stack_.begin() + start;
        auto scc_end = stack_.end();
        const unsigned limit = expansion_queue_.size();

        // Move the SCC to a new vector
        std::vector<StackInfo> scc(
            std::make_move_iterator(scc_begin),
            std::make_move_iterator(scc_end));
        stack_.erase(scc_begin, scc_end);

        // Define accessors for the SCC
        PushLocal push_local(&scc, &expansion_queue_, &stack_, &stats_);
        StateInfoLookup get_state_info(&scc, &state_infos_);
        auto get_succ_id = [start](const StateID& id) { return id - start; };

        for (unsigned i = 0; i < scc_size; ++i) {
            StateInfo& iinfo = get_state_info[scc[i].stateid];

            if (iinfo.explored) {
                continue;
            }

            push_local(i, iinfo);

            // Recursively run decomposition
            find_and_decompose_sccs<false>(
                limit,
                push_local,
                get_state_info,
                get_succ_id);
        }

        assert(stack_.size() == start);
        assert(expansion_queue_.size() == limit);
    }

    StateIDMap<State>* state_id_map_;
    RewardFunction<State, Action>* rewards_;
    TransitionGenerator<Action>* transition_gen_;

    bool expand_goals_;

    const StateEvaluator<State>* pruning_function_;

    std::unique_ptr<QuotientSystem> sys_;

    StateInfoStore state_infos_;
    ExpansionQueue expansion_queue_;
    Stack stack_;

    Statistics stats_;
};

} // namespace end_components
} // namespace probfd

#endif // __END_COMPONENT_DECOMPOSITION_H__
