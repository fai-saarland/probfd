#ifndef MDPS_END_COMPONENT_DECOMPOSITION_END_COMPONENT_DECOMPOSITION_H
#define MDPS_END_COMPONENT_DECOMPOSITION_END_COMPONENT_DECOMPOSITION_H

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
        , dead(true)
        , flag(true)
        , recurse(false)
    {
    }
    const unsigned stck;
    unsigned lstck;
    bool dead;
    bool flag;
    bool recurse;
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

    explicit StackInfo()
        : stateid(StateID::undefined)
    {
    }

    StateID stateid;
    std::vector<Action> aops;
    std::vector<std::vector<StateID>> successors;
};

struct StackInfo1 {
    explicit StackInfo1(const StateID& sid)
        : stateid(sid)
        , one(false)
        , scc_transitions(0)
    {
    }

    explicit StackInfo1()
        : stateid(StateID::undefined)
    {
    }

    StateID stateid;
    bool one;
    unsigned scc_transitions;
    std::vector<bool> active;
    std::vector<std::pair<unsigned, unsigned>> parents;
};

template <typename Action>
class StackStateIDIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = StateID;
    using difference_type = int;
    using pointer = const StateID*;
    using reference = const StateID&;

    explicit StackStateIDIterator(
        typename std::vector<StackInfo<Action>>::const_iterator it)
        : it(std::move(it))
    {
    }

    StackStateIDIterator& operator++()
    {
        ++it;
        return *this;
    }

    StackStateIDIterator operator++(int)
    {
        StackStateIDIterator res = *this;
        ++it;
        return res;
    }

    friend StackStateIDIterator operator+(
        const StackStateIDIterator& sit,
        int n)
    {
        return StackStateIDIterator(sit.it + n);
    }

    friend StackStateIDIterator operator-(
        const StackStateIDIterator& sit,
        int n)
    {
        return StackStateIDIterator(sit.it - n);
    }

    friend difference_type operator-(
        const StackStateIDIterator& a,
        const StackStateIDIterator& b)
    {
        return a.it - b.it;
    }

    reference operator[](int n)
    {
        return it[n].stateid;
    }

    bool operator==(const StackStateIDIterator& other) const
    {
        return it == other.it;
    }

    bool operator!=(const StackStateIDIterator& other) const
    {
        return it != other.it;
    }

    reference operator*() const { return it->stateid; }
    pointer operator->() const { return &it->stateid; }

protected:
    typename std::vector<StackInfo<Action>>::const_iterator it;
};

template <typename Action>
class StackAopsIterator {
public:
    using iterator_category = std::forward_iterator_tag;
    using value_type = std::vector<Action>;
    using difference_type = int;
    using pointer = const std::vector<Action>*;
    using reference = const std::vector<Action>&;

    explicit StackAopsIterator(
        typename std::vector<StackInfo<Action>>::const_iterator it)
        : it(std::move(it))
    {
    }

    StackAopsIterator& operator++()
    {
        ++it;
        return *this;
    }

    StackAopsIterator operator++(int)
    {
        StackAopsIterator res = *this;
        ++it;
        return res;
    }

    friend StackAopsIterator operator+(
        const StackAopsIterator& sit,
        int n)
    {
        return StackAopsIterator(sit.it + n);
    }

    friend StackAopsIterator operator-(
        const StackAopsIterator& sit,
        int n)
    {
        return StackAopsIterator(sit.it - n);
    }

    friend difference_type operator-(
        const StackAopsIterator& a,
        const StackAopsIterator& b)
    {
        return a.it - b.it;
    }

    reference operator[](int n)
    {
        return it[n].aops;
    }

    bool operator==(const StackAopsIterator& other) const
    {
        return it == other.it;
    }

    bool operator!=(const StackAopsIterator& other) const
    {
        return it != other.it;
    }

    reference operator*() const { return it->aops; }
    pointer operator->() const { return &it->aops; }

protected:
    typename std::vector<StackInfo<Action>>::const_iterator it;
};

using StateInfoStore = storage::PerStateStorage<StateInfo>;
template <typename Action>
using ExpansionQueue = std::deque<ExpansionInfo<Action>>;
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
 * @tparam ExpandGoalStates - Whether or not goal states should be treated as
 * terminal. If false, only states reachable before a goal state are
 * considered. If true, all reachable states are considered.
 */
template <typename State, typename Action, bool ExpandGoalStates = false>
class EndComponentDecomposition {
public:
    using QuotientSystem = quotient_system::QuotientSystem<Action>;
    using StateInfo = internal::StateInfo;
    using ExpansionInfo = internal::ExpansionInfo<Action>;
    using StackInfo = internal::StackInfo<Action>;
    using StateInfoStore = internal::StateInfoStore;
    using ExpansionQueue = internal::ExpansionQueue<Action>;
    using Stack = internal::Stack<Action>;
    using StackStateIDIterator = internal::StackStateIDIterator<Action>;
    using StackAopsIterator = internal::StackAopsIterator<Action>;
    using StackInfo1 = internal::StackInfo1;
    using Stack1 = internal::Stack1;

    EndComponentDecomposition(
        const StateEvaluator<State>* pruning_function,
        ActionIDMap<Action>* action_id_map,
        StateIDMap<State>* state_id_map,
        StateRewardFunction<State>* goal,
        ApplicableActionsGenerator<Action>* aops_gen,
        TransitionGenerator<Action>* transition_gen)
        : action_id_map_(action_id_map)
        , state_id_map_(state_id_map)
        , pruning_function_(pruning_function)
        , goal_(goal)
        , aops_gen_(aops_gen)
        , transition_gen_(transition_gen)
        , sys_(nullptr)
        , state_infos_()
        , expansion_queue_()
        , stack_()
        , transition_()
        , stats_()
    {
    }

    template <
        typename ZeroStateIDSet = const void*,
        typename OneStateIDSet = const void*>
    QuotientSystem* build_quotient_system(
        const State& initial_state,
        ZeroStateIDSet zero_states = nullptr,
        OneStateIDSet one_states = nullptr)
    {
        sys_ = new QuotientSystem(action_id_map_, aops_gen_, transition_gen_);
        stats_ = Statistics();

        if constexpr (Store<OneStateIDSet>) {
            q_aops_gen_ = new ApplicableActionsGenerator<
                quotient_system::QuotientAction<Action>>(sys_);
            q_transition_gen_ = new TransitionGenerator<
                quotient_system::QuotientAction<Action>>(sys_);
        }

        push_state(initial_state, zero_states, one_states);

        GetSuccID get_succ_id;
        auto pushf = [this, &zero_states, &one_states](
                         const StateID& state_id,
                         StateInfo& state_info) {
            return push(state_id, state_info, zero_states, one_states);
        };

        find_and_decompose_sccs<true, ZeroStateIDSet, OneStateIDSet>(
            0,
            pushf,
            state_infos_,
            get_succ_id,
            zero_states,
            one_states);

        assert(stack_.empty());
        assert(expansion_queue_.empty());
        stats_.time.stop();

        if constexpr (Store<OneStateIDSet>) {
            delete (q_aops_gen_);
            delete (q_transition_gen_);
        }

        return sys_;
    }

    void print_statistics(std::ostream& out) const { stats_.print(out); }

    Statistics get_statistics() const { return stats_; }

private:
    template <typename ZeroStateIDSet, typename OneStateIDSet>
    bool push_state(
        const State& s,
        ZeroStateIDSet zero_states,
        OneStateIDSet one_states)
    {
        const StateID state_id = this->state_id_map_->get_state_id(s);
        StateInfo& state_info = state_infos_[state_id];
        return push<ZeroStateIDSet, OneStateIDSet>(
            state_id,
            state_info,
            zero_states,
            one_states);
    }

    template <typename ZeroStateIDSet, typename OneStateIDSet>
    bool push(
        const StateID& state_id,
        StateInfo& state_info,
        ZeroStateIDSet zero_states,
        OneStateIDSet one_states)
    {
        state_info.explored = 1;
        State state = state_id_map_->get_state(state_id);

        if (goal_->operator()(state)) {
            ++stats_.terminals;
            ++stats_.goals;
            ++stats_.ones;
            insert(one_states, state_id);
            state_info.stackid_ = StateInfo::ONE;
            if (!ExpandGoalStates) {
                return false;
            }

            state_info.expandable_goal = 1;
        } else if (
            pruning_function_ != nullptr &&
            pruning_function_->operator()(state)) {
            ++stats_.terminals;
            insert(zero_states, state_id);
            state_info.stackid_ = StateInfo::ZERO;
            return false;
        }

        std::vector<Action> aops;
        aops_gen_->operator()(state_id, aops);

        if (aops.empty()) {
            if (ExpandGoalStates && state_info.expandable_goal) {
                state_info.expandable_goal = 0;
            } else {
                ++stats_.terminals;
                state_info.stackid_ = StateInfo::ZERO;
                insert(zero_states, state_id);
            }

            return false;
        }

        std::vector<std::vector<StateID>> successors;
        successors.reserve(aops.size());

        unsigned non_loop_actions = 0;
        for (unsigned i = 0; i < aops.size(); ++i) {
            transition_gen_->operator()(state_id, aops[i], transition_);
            std::vector<StateID> succ_ids;

            for (const StateID& succ_id : transition_.elements()) {
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

            transition_.clear();
        }

        // only self-loops
        if (non_loop_actions == 0) {
            if (ExpandGoalStates && state_info.expandable_goal) {
                state_info.expandable_goal = 0;
            } else {
                ++stats_.terminals;
                ++stats_.selfloops;
                state_info.stackid_ = StateInfo::ZERO;
                insert(zero_states, state_id);
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

    struct GetSuccID {
        StateID operator()(const StateID& id) const { return id; }
        StateID operator()(const StackInfo& sinfo) const
        {
            return sinfo.stateid;
        }
    };

    struct SuccIDToLocal {
        explicit SuccIDToLocal(const unsigned start)
            : start_(start)
        {
        }

        unsigned operator()(const StateID& state_id) const
        {
            return state_id - start_;
        }

        const unsigned start_;
    };

    template <typename T>
    static constexpr bool Store = !std::is_same_v<T, const void*>;

    template <
        bool RootIteration,
        typename ZeroStateIDSet,
        typename OneStateIDSet,
        typename Push,
        typename GetStateInfo,
        typename GetSuccID>
    void find_and_decompose_sccs(
        const unsigned limit,
        Push& pushf,
        GetStateInfo& get_state_info,
        GetSuccID& get_state_id,
        ZeroStateIDSet zero_states,
        OneStateIDSet one_states)
    {
        // Whether we backtracked from a successor state
        bool backtracked = false;

        // Data of the backtracked successor state
        unsigned lstck = 0;
        bool dead = true;
        bool recurse = false;
        bool is_onstack = false;

        while (expansion_queue_.size() > limit) {
            ExpansionInfo& e = expansion_queue_.back();
            StackInfo& s = stack_[e.stck];

            if (backtracked) {
                if (is_onstack) {
                    e.lstck = std::min(e.lstck, lstck);
                    e.recurse |= recurse || !e.flag;
                } else {
                    e.recurse |= e.flag && s.successors.back().size() > 1;
                    e.flag = false;
                }

                if constexpr (Store<ZeroStateIDSet>) {
                    e.dead &= dead;
                }

                if (e.successors.back().empty()) {
                    e.successors.pop_back();

                    if (e.flag) {
                        assert(!s.successors.back().empty());
                        s.aops.push_back(e.aops.back());
                        s.successors.emplace_back();
                    } else {
                        e.flag = true;
                        s.successors.back().clear();
                    }
                    e.aops.pop_back();
                }
            }

            backtracked = !successor_loop<
                ZeroStateIDSet,
                OneStateIDSet,
                Push,
                GetStateInfo,
                GetSuccID>(e, s, pushf, get_state_info, get_state_id);

            if (!backtracked) {
                continue;
            }

            assert(e.successors.empty());
            assert(e.aops.empty());

            recurse = e.recurse;
            lstck = e.lstck;
            dead = e.dead;
            is_onstack = e.stck != e.lstck;

            assert(e.stck >= e.lstck);

            assert(s.successors.back().empty());
            s.successors.pop_back();

            if (e.stck == e.lstck) {
                scc_found<
                    RootIteration,
                    ZeroStateIDSet,
                    OneStateIDSet,
                    GetStateInfo>(
                    recurse,
                    e,
                    s,
                    get_state_info,
                    zero_states,
                    one_states);
            }

            expansion_queue_.pop_back();
        }
    }

    template <
        typename ZeroStateIDSet,
        typename OneStateIDSet,
        typename Push,
        typename GetStateInfo,
        typename GetSuccID>
    bool successor_loop(
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
                    e.recurse |= e.flag && s_succs.size() > 1;
                    e.flag = false;

                    if (Store<ZeroStateIDSet> && succ_info.one()) {
                        e.dead = false;
                    }
                } else if (succ_info.onstack()) {
                    e.recurse = e.recurse || !e.flag;
                    e.lstck = std::min(e.lstck, succ_info.stackid());
                    s_succs.emplace_back(succ_info.stackid());
                } else {
                    e.recurse |= e.flag && !s_succs.empty();
                    e.flag = false;

                    if (Store<ZeroStateIDSet> && !succ_info.zero()) {
                        e.dead = false;
                    }
                }
            } while (!e_succs.empty());

            assert(e_succs.empty());
            e.successors.pop_back();

            if (e.flag) {
                assert(!s_succs.empty());
                s.successors.emplace_back();
                s.aops.push_back(e.aops.back());
            } else {
                s_succs.clear();
                e.flag = true;
            }

            e.aops.pop_back();
        }

        return false;
    }

    template <
        bool RootIteration,
        typename ZeroStateIDSet,
        typename OneStateIDSet,
        typename GetStateInfo>
    void scc_found(
        bool& recurse,
        ExpansionInfo& e,
        StackInfo& s,
        GetStateInfo& get_state_info,
        ZeroStateIDSet zero_states,
        OneStateIDSet one_states)
    {
        unsigned scc_size = stack_.size() - e.stck;
        auto scc_begin = stack_.begin() + e.stck;
        auto scc_end = stack_.end();

        if (Store<ZeroStateIDSet> && e.dead) {
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
                insert(zero_states, it->stateid);
            }

            stack_.resize(e.stck);
        } else {
            StateID scc_repr_id = s.stateid;
            if (scc_size == 1) {
                assert(s.aops.empty());
                StateInfo& info = get_state_info[scc_repr_id];
                info.stackid_ = StateInfo::UNDEF;

                if constexpr (Store<OneStateIDSet>) {
                    info.explored = 0;
                }

                stack_.pop_back();
                ++stats_.ec1;

                if constexpr (RootIteration) {
                    ++stats_.sccs1;
                }
            } else {
                if (ExpandGoalStates) {
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

                    decompose<Store<OneStateIDSet>>(e.stck);
                } else {
                    unsigned transitions = 0;

                    for (auto it = scc_begin; it != scc_end; ++it) {
                        assert(it->successors.size() == it->aops.size());
                        StateInfo& info = get_state_info[it->stateid];
                        info.stackid_ = StateInfo::UNDEF;

                        if (Store<OneStateIDSet>) {
                            info.explored = 0;
                        }

                        transitions += it->aops.size();
                    }

                    StackStateIDIterator begin(scc_begin);
                    StackStateIDIterator end(scc_end);
                    StackAopsIterator abegin(scc_begin);
                    sys_->build_quotient(begin, end, scc_repr_id, abegin);
                    stack_.erase(scc_begin, scc_end);
                    ++stats_.eck;
                    stats_.ec_transitions += transitions;

                    if constexpr (RootIteration) {
                        ++stats_.sccsk;
                    }
                }
            }

            if constexpr (Store<OneStateIDSet>) {
                collect_one_states<OneStateIDSet>(one_states, scc_repr_id);
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
        SuccIDToLocal get_succ_id(start);

        for (unsigned i = 0; i < scc_size; ++i) {
            StateInfo& iinfo = get_state_info[i];

            if (iinfo.explored) {
                continue;
            }

            push_local(i, iinfo);

            // Recursively run decomposition
            find_and_decompose_sccs<false, const void*, const void*>(
                limit,
                push_local,
                get_state_info,
                get_succ_id,
                nullptr,
                nullptr);
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

        ExpansionInfo& e = expansion_queue_.emplace_back(stack1_.size());
        e.dead = !state_info.expandable_goal;
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
        stack1_.emplace_back(state_id);

        if (ExpandGoalStates && state_info.expandable_goal) {
            stack1_.back().one = 1;
        }
    }

    template <typename OneStateIDSet>
    void
    collect_one_states(OneStateIDSet one_states, const StateID& source_state)
    {
        const unsigned limit = expansion_queue_.size();
        push1(source_state, state_infos_[source_state]);

        bool backtracked = false;
        bool one = false;
        unsigned lstck = 0;
        bool onstack = false;
        StackInfo1* backtracked_from = nullptr;

        while (expansion_queue_.size() > limit) {
            ExpansionInfo& e = expansion_queue_.back();
            StackInfo1& s = stack1_[e.stck];

            if (backtracked) {
                if (onstack) {
                    e.lstck = std::min(e.lstck, lstck);
                    e.recurse = true;
                    if (!ExpandGoalStates || e.dead) {
                        backtracked_from->parents.emplace_back(
                            e.stck,
                            s.active.size());
                    }
                } else {
                    e.flag &= one;
                }

                if (e.successors.back().empty()) {
                    e.successors.pop_back();

                    if (e.recurse) {
                        s.scc_transitions += e.flag;
                        s.active.push_back(e.flag);
                    } else if (e.flag) {
                        s.one = true;
                    }

                    e.flag = true;
                    e.recurse = false;
                }
            }

            backtracked = true;
            while (!e.successors.empty()) {
                std::vector<StateID>& succs = e.successors.back();

                while (!succs.empty()) {
                    StateID succ_id = succs.back();
                    succs.pop_back();
                    StateInfo& succ_info = state_infos_[succ_id];

                    if (!succ_info.explored) {
                        push1(succ_id, succ_info);
                        backtracked = false;
                        break;
                    }

                    if (succ_info.onstack()) {
                        e.recurse = true;
                        e.lstck = std::min(e.lstck, succ_info.stackid_);
                        if (!ExpandGoalStates || e.dead) {
                            stack1_[succ_info.stackid_].parents.emplace_back(
                                e.stck,
                                s.active.size());
                        }
                    } else if (!succ_info.one()) {
                        e.flag = false;
                    }
                }

                if (!backtracked) {
                    break;
                }

                assert(succs.empty());
                e.successors.pop_back();

                if (e.recurse) {
                    s.scc_transitions += e.flag;
                    s.active.push_back(e.flag);
                } else if (e.flag) {
                    s.one = true;
                }

                e.flag = true;
                e.recurse = false;
            }

            if (!backtracked) {
                continue;
            }

            one = s.one;
            lstck = e.lstck;
            backtracked_from = &s;
            onstack = e.stck != e.lstck;

            if (!onstack) {
                auto begin = stack1_.begin() + e.stck;
                auto end = stack1_.end();

                std::deque<StackInfo1*> non_one;
                {
                    for (auto stk_it = begin; stk_it != end; ++stk_it) {
                        if (!stk_it->one && !stk_it->scc_transitions &&
                            !stk_it->parents.empty()) {
                            non_one.push_back(&*stk_it);
                        }
                    }
                }

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

                for (auto stk_it = begin; stk_it != end; ++stk_it) {
                    auto qstates = sys_->quotient_range(stk_it->stateid);

                    if (stk_it->one || stk_it->scc_transitions) {
                        for (const StateID& sid : qstates) {
                            StateInfo& sinfo = state_infos_[sid];
                            sinfo.explored = 1;
                            sinfo.stackid_ = StateInfo::ONE;
                            insert(one_states, sid);
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

            expansion_queue_.pop_back();
        }
    }

    inline void insert(const void*, const StateID&) {}
    inline void insert(storage::PerStateStorage<bool>& x, const StateID& y)
    {
        x[y] = true;
    }
    inline void insert(storage::StateIDHashSet& x, const StateID& y)
    {
        x.insert(y);
    }
    inline void insert(pdbs::QualitativeResultStore& x, const StateID& y)
    {
        x[y] = true;
    }

    ActionIDMap<Action>* action_id_map_;
    StateIDMap<State>* state_id_map_;
    const StateEvaluator<State>* pruning_function_;
    StateRewardFunction<State>* goal_;
    ApplicableActionsGenerator<Action>* aops_gen_;
    TransitionGenerator<Action>* transition_gen_;

    QuotientSystem* sys_;
    ApplicableActionsGenerator<typename QuotientSystem::QAction>* q_aops_gen_;
    TransitionGenerator<typename QuotientSystem::QAction>* q_transition_gen_;

    StateInfoStore state_infos_;
    ExpansionQueue expansion_queue_;
    Stack stack_;
    Stack1 stack1_;

    Distribution<StateID> transition_;

    Statistics stats_;
};

} // namespace end_components
} // namespace probabilistic

#endif // __END_COMPONENT_DECOMPOSITION_H__