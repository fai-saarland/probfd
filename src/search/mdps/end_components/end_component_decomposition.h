#ifndef MDPS_END_COMPONENT_DECOMPOSITION_END_COMPONENT_DECOMPOSITION_H
#define MDPS_END_COMPONENT_DECOMPOSITION_END_COMPONENT_DECOMPOSITION_H

#include "../../utils/iterators.h"
#include "../../utils/timer.h"
#include "../engine_interfaces/action_id_map.h"
#include "../engine_interfaces/applicable_actions_generator.h"
#include "../engine_interfaces/state_evaluator.h"
#include "../engine_interfaces/state_id_map.h"
#include "../engine_interfaces/state_reward_function.h"
#include "../engine_interfaces/transition_generator.h"
#include "../heuristics/pdbs/qualitative_result_store.h"
#include "../quotient_system/engine_interfaces.h"
#include "../quotient_system/quotient_system.h"
#include "../storage/per_state_storage.h"

#include <cassert>
#include <deque>
#include <iostream>
#include <iterator>
#include <type_traits>
#include <vector>

namespace probabilistic {

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
        out << "  " << ones << " state(s) can reach the goal with certainty"
            << std::endl;
    }

    unsigned long long goals = 0;
    unsigned long long terminals = 0;
    unsigned long long selfloops = 0;

    unsigned long long sccs1 = 0;
    unsigned long long sccsk = 0;
    unsigned long long sccs1_dead = 0;
    unsigned long long sccsk_dead = 0;
    unsigned long long ones = 0;

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
    bool zero() const { return stackid_ == ZERO; }
    bool one() const { return stackid_ == ONE; }

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

    // Dead-End flag
    bool dead = true;

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
struct ExpansionInfo1 {
    explicit ExpansionInfo1(unsigned stck, bool non_expandable_goal)
        : stck(stck)
        , lstck(stck)
        , non_expandable_goal(non_expandable_goal)
    {
    }

    // Tarjan's SCC algorithm: stack id and lowlink
    const unsigned stck;
    unsigned lstck;

    const bool non_expandable_goal;

    bool exits_only_proper = true;
    bool transitions_in_scc = false;

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

struct StackInfo1 {
    explicit StackInfo1(const StateID& sid)
        : stateid(sid)
    {
    }

    StateID stateid;

    bool one = false;
    unsigned scc_transitions = 0;

    std::vector<bool> active;
    std::vector<std::pair<unsigned, unsigned>> parents;
};

using StateInfoStore = storage::PerStateStorage<StateInfo>;
template <typename Action>
using ExpansionQueue = std::deque<ExpansionInfo<Action>>;
template <typename Action>
using ExpansionQueue1 = std::deque<ExpansionInfo1<Action>>;
template <typename Action>
using Stack = std::vector<StackInfo<Action>>;
using Stack1 = std::vector<StackInfo1>;

} // namespace internal

/**
 * @brief A builder class that implements end component decomposition.
 *
 * An end component is a non-empty subgraph of an MDP which:
 * - is strongly connected
 * - has at least one enabled action for every state
 * - is closed under probabilistic branching
 *
 * A maximal end component (MEC) is an end component that is maximal with
 * respect to set inclusion.
 *
 * In reachability analysis (i.e. MaxProb) the MEC decomposition builds the
 * maximal end components of the underlying MDP and constructs a quotient
 * by considering each MEC as a single state. All states of an MEC have the
 * same optimal state value, hence this transformation is sound with respect to
 * MaxProb.
 *
 * In MaxProb, the optimal value function is the least fixed point of the
 * Bellman operator, but not necessarily unique. Hence value iteration
 * algorithms must start from a lower bound to achieve convergence. In the MEC
 * decomposition the fixed point is unique, thus value iteration algorithms may
 * start from any initial value function when MEC decompostion is run as a
 * preprocessing step.
 *
 * @see IntervalIteration
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 */
template <typename State, typename Action>
class EndComponentDecomposition {
public:
    using QuotientSystem = quotient_system::QuotientSystem<Action>;
    using StateInfo = internal::StateInfo;
    using ExpansionInfo = internal::ExpansionInfo<Action>;
    using StackInfo = internal::StackInfo<Action>;
    using StateInfoStore = internal::StateInfoStore;
    using ExpansionQueue = internal::ExpansionQueue<Action>;
    using ExpansionQueue1 = internal::ExpansionQueue1<Action>;
    using Stack = internal::Stack<Action>;
    using StackInfo1 = internal::StackInfo1;
    using ExpansionInfo1 = internal::ExpansionInfo1<Action>;
    using Stack1 = internal::Stack1;

    EndComponentDecomposition(
        const StateEvaluator<State>* pruning_function,
        ActionIDMap<Action>* action_id_map,
        StateIDMap<State>* state_id_map,
        StateRewardFunction<State>* goal,
        ActionRewardFunction<Action>* action_rewards,
        ApplicableActionsGenerator<Action>* aops_gen,
        TransitionGenerator<Action>* transition_gen,
        bool expand_goals)
        : pruning_function_(pruning_function)
        , action_id_map_(action_id_map)
        , state_id_map_(state_id_map)
        , goal_(goal)
        , action_rewards_(action_rewards)
        , aops_gen_(aops_gen)
        , transition_gen_(transition_gen)
        , expand_goals_(expand_goals)
        , sys_(new QuotientSystem(action_id_map_, aops_gen_, transition_gen_))
    {
    }

    template <typename ZeroOutputIt, typename OneOutputIt>
    std::unique_ptr<QuotientSystem> build_quotient_system(
        const State& initial_state,
        ZeroOutputIt zero_states_out = utils::discarding_output_iterator{},
        OneOutputIt one_states_out = utils::discarding_output_iterator{})
    {
        stats_ = Statistics();

        if constexpr (Store<OneOutputIt>) {
            q_aops_gen_ = new ApplicableActionsGenerator<
                quotient_system::QuotientAction<Action>>(sys_.get());
            q_transition_gen_ = new TransitionGenerator<
                quotient_system::QuotientAction<Action>>(sys_.get());
        }

        push_state(initial_state, zero_states_out, one_states_out);

        auto get_succ_id = [](const StateID& id) { return id; };
        auto pushf = [this, zero_states_out, one_states_out](
                         const StateID& state_id,
                         StateInfo& state_info) {
            return push(state_id, state_info, zero_states_out, one_states_out);
        };

        find_and_decompose_sccs<true>(
            0,
            pushf,
            state_infos_,
            get_succ_id,
            zero_states_out,
            one_states_out);

        assert(stack_.empty());
        assert(expansion_queue_.empty());
        stats_.time.stop();

        if constexpr (Store<OneOutputIt>) {
            delete (q_aops_gen_);
            delete (q_transition_gen_);
        }

        return std::move(sys_);
    }

    void print_statistics(std::ostream& out) const { stats_.print(out); }

    Statistics get_statistics() const { return stats_; }

private:
    template <typename ZeroOutputIt, typename OneOutputIt>
    bool push_state(
        const State& s,
        ZeroOutputIt zero_states_out,
        OneOutputIt one_states_out)
    {
        const StateID state_id = this->state_id_map_->get_state_id(s);
        StateInfo& state_info = state_infos_[state_id];
        return push(state_id, state_info, zero_states_out, one_states_out);
    }

    template <typename ZeroOutputIt, typename OneOutputIt>
    bool push(
        const StateID& state_id,
        StateInfo& state_info,
        ZeroOutputIt zero_states_out,
        OneOutputIt one_states_out)
    {
        state_info.explored = 1;
        State state = state_id_map_->get_state(state_id);

        if (goal_->operator()(state)) {
            ++stats_.terminals;
            ++stats_.goals;
            ++stats_.ones;
            *one_states_out = state_id;
            state_info.stackid_ = StateInfo::ONE;

            if (!expand_goals_) {
                return false;
            }

            state_info.expandable_goal = 1;
        } else if (
            pruning_function_ != nullptr &&
            pruning_function_->operator()(state)) {
            ++stats_.terminals;
            *zero_states_out = state_id;
            state_info.stackid_ = StateInfo::ZERO;
            return false;
        }

        std::vector<Action> aops;
        aops_gen_->operator()(state_id, aops);

        if (aops.empty()) {
            if (expand_goals_ && state_info.expandable_goal) {
                state_info.expandable_goal = 0;
            } else {
                ++stats_.terminals;
                state_info.stackid_ = StateInfo::ZERO;
                *zero_states_out = state_id;
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
                *zero_states_out = state_id;
            }

            return false;
        }

        aops.erase(aops.begin() + non_loop_actions, aops.end());

        ExpansionInfo& e = expansion_queue_.emplace_back(stack_.size());
        e.dead = !state_info.expandable_goal;
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

    template <typename T>
    static constexpr bool Store =
        !std::is_same_v<T, utils::discarding_output_iterator>;

    template <
        bool RootIteration,
        typename ZeroOutputIt,
        typename OneOutputIt,
        typename Push,
        typename GetStateInfo,
        typename GetSuccID>
    void find_and_decompose_sccs(
        const unsigned limit,
        Push& pushf,
        GetStateInfo& get_state_info,
        GetSuccID& get_state_id,
        ZeroOutputIt zero_states_out,
        OneOutputIt one_states_out)
    {
        if (expansion_queue_.size() <= limit) {
            return;
        }

        ExpansionInfo* e = &expansion_queue_.back();
        StackInfo* s = &stack_[e->stck];

        for (;;) {
            // DFS recursion
            while (push_successor<ZeroOutputIt, OneOutputIt>(
                *e,
                *s,
                pushf,
                get_state_info,
                get_state_id)) {
                e = &expansion_queue_.back();
                s = &stack_[e->stck];
            }

            assert(e->successors.empty() && e->aops.empty());
            assert(e->stck >= e->lstck);
            assert(s->successors.back().empty());

            s->successors.pop_back();

            bool recurse = e->recurse;
            unsigned int lstck = e->lstck;
            bool dead = e->dead;
            bool is_onstack = e->stck != e->lstck;

            if (e->stck == e->lstck) {
                scc_found<RootIteration>(
                    recurse,
                    *e,
                    *s,
                    get_state_info,
                    zero_states_out,
                    one_states_out);
            }

            expansion_queue_.pop_back();

            if (expansion_queue_.size() <= limit) {
                break;
            }

            e = &expansion_queue_.back();
            s = &stack_[e->stck];

            // We returned from a recursive DFS call. Update the parent.

            const auto action_reward =
                (*action_rewards_)(get_state_id(s->stateid), e->aops.back());

            if (is_onstack) {
                e->lstck = std::min(e->lstck, lstck);
                e->recurse = e->recurse || recurse || !e->remains_in_scc ||
                             action_reward != value_type::zero;
            } else {
                e->recurse = e->recurse || (e->remains_in_scc &&
                                            s->successors.back().size() > 1);
                e->remains_in_scc = false;
            }

            if constexpr (Store<ZeroOutputIt>) {
                e->dead &= dead;
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

    template <
        typename ZeroOutputIt,
        typename OneOutputIt,
        typename Push,
        typename GetStateInfo,
        typename GetSuccID>
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
                (*action_rewards_)(get_state_id(s.stateid), e.aops.back());

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

                    if (Store<OneOutputIt> && succ_info.one()) {
                        e.dead = false;
                    }
                } else if (succ_info.onstack()) {
                    s_succs.emplace_back(succ_info.stackid());
                    e.lstck = std::min(e.lstck, succ_info.stackid());
                    e.recurse = e.recurse || !e.remains_in_scc ||
                                action_reward != value_type::zero;
                } else {
                    e.recurse =
                        e.recurse || (e.remains_in_scc && !s_succs.empty());
                    e.remains_in_scc = false;

                    if (Store<ZeroOutputIt> && !succ_info.zero()) {
                        e.dead = false;
                    }
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

    template <
        bool RootIteration,
        typename ZeroOutputIt,
        typename OneOutputIt,
        typename GetStateInfo>
    void scc_found(
        bool& recurse,
        ExpansionInfo& e,
        StackInfo& s,
        GetStateInfo& get_state_info,
        ZeroOutputIt zero_states_out,
        OneOutputIt one_states_out)
    {
        unsigned scc_size = stack_.size() - e.stck;
        auto scc_begin = stack_.begin() + e.stck;
        auto scc_end = stack_.end();

        if (Store<ZeroOutputIt> && e.dead) {
            if constexpr (RootIteration) {
                const bool singleton = (scc_size == 1);
                stats_.sccs1 += singleton;
                stats_.sccs1_dead += singleton;
                stats_.sccsk += !singleton;
                stats_.sccsk_dead += !singleton;
            }

            for (auto it = scc_begin; it != scc_end; ++it) {
                StateInfo& info = get_state_info[it->stateid];
                info.stackid_ = StateInfo::ZERO;
                *zero_states_out = it->stateid;
            }

            stack_.erase(scc_begin, scc_end);
        } else {
            StateID scc_repr_id = s.stateid;
            if (scc_size == 1) {
                assert(s.aops.empty());
                StateInfo& info = get_state_info[scc_repr_id];
                info.stackid_ = StateInfo::UNDEF;

                if constexpr (Store<OneOutputIt>) {
                    info.explored = 0;
                }

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
                            recurse = true;
                        }
                    }
                }

                if (recurse) {
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

                    decompose<Store<OneOutputIt>>(e.stck);
                } else {
                    unsigned transitions = 0;

                    for (auto it = scc_begin; it != scc_end; ++it) {
                        assert(it->successors.size() == it->aops.size());
                        StateInfo& info = get_state_info[it->stateid];
                        info.stackid_ = StateInfo::UNDEF;

                        if (Store<OneOutputIt>) {
                            info.explored = 0;
                        }

                        transitions += it->aops.size();
                    }

                    auto begin = utils::make_transform_iterator(
                        scc_begin,
                        &StackInfo::stateid);
                    auto end = utils::make_transform_iterator(
                        scc_end,
                        &StackInfo::stateid);
                    auto abegin = utils::make_transform_iterator(
                        scc_begin,
                        &StackInfo::aops);
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

            if constexpr (Store<OneOutputIt>) {
                collect_one_states<OneOutputIt>(one_states_out, scc_repr_id);
            }
        }

        assert(stack_.size() == e.stck);
    }

    template <bool ResetExplored = false>
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
                get_succ_id,
                utils::discarding_output_iterator{},
                utils::discarding_output_iterator{});
        }

        if constexpr (ResetExplored) {
            for (const StackInfo& info : scc) {
                state_infos_[info.stateid].explored = 0;
            }
        }

        assert(stack_.size() == start);
        assert(expansion_queue_.size() == limit);
    }

    void push1(const StateID& state_id, StateInfo& state_info)
    {
        assert(!state_info.explored);
        assert(!state_info.one() && !state_info.zero());

        std::vector<typename QuotientSystem::QAction> aops;
        q_aops_gen_->operator()(state_id, aops);

        assert(!aops.empty());

        ExpansionInfo1& e = expansion_queue1_.emplace_back(
            stack1_.size(),
            !state_info.expandable_goal);

        e.successors.reserve(aops.size());

        for (const auto& action : aops) {
            Distribution<StateID> transition;
            q_transition_gen_->operator()(state_id, action, transition);

            std::vector<StateID> succs;

            for (const StateID& succ_id : transition.elements()) {
                if (succ_id != state_id) {
                    succs.push_back(succ_id);
                }
            }

            if (!succs.empty()) {
                e.successors.push_back(std::move(succs));
            }
        }

        assert(!e.successors.empty());

        state_info.stackid_ = stack1_.size();
        state_info.explored = 1;

        StackInfo1& info = stack1_.emplace_back(state_id);
        info.one = expand_goals_ && state_info.expandable_goal;
    }

    template <typename OneOutputIt>
    void
    collect_one_states(OneOutputIt one_states_out, const StateID& source_state)
    {
        assert(expansion_queue1_.empty());

        push1(source_state, state_infos_[source_state]);

        assert(!expansion_queue1_.empty());

        ExpansionInfo1* e = &expansion_queue1_.back();
        StackInfo1* s = &stack1_[e->stck];

        for (;;) {
            // DFS recursion
            while (push_successor1(*e, *s)) {
                e = &expansion_queue1_.back();
                s = &stack1_[e->stck];
            }

            bool one = s->one;
            unsigned lstck = e->lstck;
            StackInfo1* backtracked_from = s;
            bool onstack = e->stck != e->lstck;

            if (!onstack) {
                scc_found1(
                    stack1_.begin() + e->stck,
                    stack1_.end(),
                    one_states_out);
            }

            expansion_queue1_.pop_back();

            if (expansion_queue1_.empty()) {
                break;
            }

            e = &expansion_queue1_.back();
            s = &stack1_[e->stck];

            // We returned from a recursive DFS call. Update the parent.

            if (onstack) {
                e->lstck = std::min(e->lstck, lstck);
                e->transitions_in_scc = true;

                if (!expand_goals_ || e->non_expandable_goal) {
                    auto& parents = backtracked_from->parents;
                    parents.emplace_back(e->stck, s->active.size());
                }
            } else {
                e->exits_only_proper = e->exits_only_proper && one;
            }

            if (e->successors.back().empty()) {
                e->successors.pop_back();

                if (e->transitions_in_scc) {
                    if (e->exits_only_proper) ++s->scc_transitions;
                    s->active.push_back(e->exits_only_proper);
                } else if (e->exits_only_proper) {
                    s->one = true;
                }

                // Reset transition flags
                e->exits_only_proper = true;
                e->transitions_in_scc = false;
            }
        }
    }

    bool push_successor1(ExpansionInfo1& e, StackInfo1& s)
    {
        while (!e.successors.empty()) {
            std::vector<StateID>& succs = e.successors.back();

            while (!succs.empty()) {
                StateID succ_id = succs.back();
                succs.pop_back();
                StateInfo& succ_info = state_infos_[succ_id];

                if (!succ_info.explored) {
                    push1(succ_id, succ_info);
                    return true;
                }

                if (succ_info.onstack()) {
                    unsigned succ_stack_id = succ_info.stackid_;
                    e.transitions_in_scc = true;
                    e.lstck = std::min(e.lstck, succ_stack_id);

                    if (!expand_goals_ || e.non_expandable_goal) {
                        auto& parents = stack1_[succ_stack_id].parents;
                        parents.emplace_back(e.stck, s.active.size());
                    }
                } else if (!succ_info.one()) {
                    e.exits_only_proper = false;
                }
            }

            assert(succs.empty());

            if (e.transitions_in_scc) {
                if (e.exits_only_proper) ++s.scc_transitions;
                s.active.push_back(e.exits_only_proper);
            } else if (e.exits_only_proper) {
                s.one = true;
            }

            // Reset transition flags
            e.exits_only_proper = true;
            e.transitions_in_scc = false;

            e.successors.pop_back();
        }

        return false;
    }

    template <typename OneOutputIt>
    void scc_found1(
        decltype(std::declval<Stack1>().begin()) begin,
        decltype(std::declval<Stack1>().begin()) end,
        OneOutputIt one_states_out)
    {
        // Collect non-proper states
        std::deque<StackInfo1*> non_one;
        {
            for (auto stk_it = begin; stk_it != end; ++stk_it) {
                if (!stk_it->one && stk_it->scc_transitions == 0 &&
                    !stk_it->parents.empty()) {
                    non_one.push_back(&*stk_it);
                }
            }
        }

        // Fix-point iteration to throw out actions which do not
        // uniformly lead to proper states. If no action remains the state
        // is not proper.
        while (!non_one.empty()) {
            StackInfo1* scc_elem = non_one.back();
            non_one.pop_back();

            for (const auto& [first, second] : scc_elem->parents) {
                StackInfo1& pinfo = stack1_[first];
                if (!pinfo.one && pinfo.active[second]) {
                    pinfo.active[second] = false;
                    if (--pinfo.scc_transitions == 0 &&
                        !pinfo.parents.empty()) {
                        non_one.push_back(&pinfo);
                    }
                }
            }
        }

        // Report the proper states
        for (auto stk_it = begin; stk_it != end; ++stk_it) {
            auto qstates = sys_->quotient_range(stk_it->stateid);

            if (stk_it->one || stk_it->scc_transitions) {
                for (const StateID& sid : qstates) {
                    StateInfo& sinfo = state_infos_[sid];
                    sinfo.explored = 1;
                    sinfo.stackid_ = StateInfo::ONE;
                    *one_states_out = sid;
                    ++stats_.ones;
                }
            } else {
                for (const StateID& sid : qstates) {
                    StateInfo& sinfo = state_infos_[sid];
                    sinfo.explored = 1;
                    sinfo.stackid_ = StateInfo::UNDEF;
                }
            }
        }

        stack1_.erase(begin, end);
    }

    const StateEvaluator<State>* pruning_function_;
    ActionIDMap<Action>* action_id_map_;
    StateIDMap<State>* state_id_map_;
    StateRewardFunction<State>* goal_;
    ActionRewardFunction<Action>* action_rewards_;
    ApplicableActionsGenerator<Action>* aops_gen_;
    TransitionGenerator<Action>* transition_gen_;

    bool expand_goals_;

    std::unique_ptr<QuotientSystem> sys_;
    ApplicableActionsGenerator<typename QuotientSystem::QAction>* q_aops_gen_;
    TransitionGenerator<typename QuotientSystem::QAction>* q_transition_gen_;

    StateInfoStore state_infos_;
    ExpansionQueue expansion_queue_;
    Stack stack_;

    ExpansionQueue1 expansion_queue1_;
    Stack1 stack1_;

    Statistics stats_;
};

} // namespace end_components
} // namespace probabilistic

#endif // __END_COMPONENT_DECOMPOSITION_H__