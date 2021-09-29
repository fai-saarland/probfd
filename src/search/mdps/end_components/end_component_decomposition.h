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
        , flag(0)
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
    unsigned flag : 1;
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
        if (Store<OneStateIDSet>::value) {
            q_aops_gen_ = new ApplicableActionsGenerator<
                quotient_system::QuotientAction<Action>>(sys_);
            q_transition_gen_ = new TransitionGenerator<
                quotient_system::QuotientAction<Action>>(sys_);
        }
        push_state<ZeroStateIDSet, OneStateIDSet>(
            initial_state,
            zero_states,
            one_states);
        GetSuccID get_succ_id;
        auto pushf = [this, &zero_states, &one_states](
                         const StateID& state_id,
                         StateInfo& state_info) {
            return push<ZeroStateIDSet, OneStateIDSet>(
                state_id,
                state_info,
                zero_states,
                one_states);
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
        if (Store<OneStateIDSet>::value) {
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
            state_info.flag = 1;
        } else if ((pruning_function_ != nullptr &&
                    pruning_function_->operator()(state))) {
            ++stats_.terminals;
            insert(zero_states, state_id);
            state_info.stackid_ = StateInfo::ZERO;
            return false;
        }
        std::vector<Action> aops;
        aops_gen_->operator()(state_id, aops);
        if (aops.empty()) {
            if (ExpandGoalStates && state_info.flag) {
                state_info.flag = 0;
            } else {
                ++stats_.terminals;
                state_info.stackid_ = StateInfo::ZERO;
                insert(zero_states, state_id);
            }
            return false;
        }
        std::vector<std::vector<StateID>> successors;
        successors.reserve(aops.size());
        unsigned n = 0;
        for (unsigned i = 0; i < aops.size(); ++i) {
            transition_gen_->operator()(state_id, aops[i], transition_);
            std::vector<StateID> succ_ids;
            for (auto it = transition_.begin(); it != transition_.end(); ++it) {
                const StateID succ_id = it->first;
                if (succ_id != state_id) {
                    succ_ids.push_back(succ_id);
                }
            }
            if (!succ_ids.empty()) {
                successors.emplace_back(std::move(succ_ids));
                if (i != n) {
                    aops[n] = aops[i];
                }
                ++n;
            }
            transition_.clear();
        }
        if (n == 0) {
            if (ExpandGoalStates && state_info.flag) {
                state_info.flag = 0;
            } else {
                ++stats_.terminals;
                ++stats_.selfloops;
                state_info.stackid_ = StateInfo::ZERO;
                insert(zero_states, state_id);
            }
            return false;
        }
        while (aops.size() > n)
            aops.pop_back();
        expansion_queue_.emplace_back(stack_.size());
        ExpansionInfo& e = expansion_queue_.back();
        e.dead = !state_info.flag;
        e.aops.swap(aops);
        e.successors.swap(successors);
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
            expansion_queue_->emplace_back(stack_->size());
            ExpansionInfo& e = expansion_queue_->back();
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
    using Store =
        std::integral_constant<bool, !std::is_same<T, const void*>::value>;

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
        bool backtracked = false;
        bool recurse = false;
        unsigned lstck = 0;
        bool dead = true;
        bool is_onstack = false;
        while (expansion_queue_.size() > limit) {
            ExpansionInfo& e = expansion_queue_.back();
            StackInfo& s = stack_[e.stck];
            // std::cout << s.stateid << " (" << e.stck << "/" << stack_.size()
            // << ", " << e.lstck << ")" << " ... " << backtracked << ", " <<
            // recurse << ", " << lstck << " >> " << (lstck <= e.stck) << " : "
            // << std::min(e.lstck, lstck) << std::endl;

            if (backtracked) {
                if (is_onstack) {
                    e.lstck = std::min(e.lstck, lstck);
                    e.recurse = e.recurse || recurse || !e.flag;
                } else {
                    e.recurse =
                        e.recurse || (e.flag && s.successors.back().size() > 1);
                    e.flag = false;
                }
                if (Store<ZeroStateIDSet>::value) {
                    e.dead = e.dead && dead;
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

            backtracked = true;
            while (!e.successors.empty()) {
                std::vector<StateID>& succs = e.successors.back();
                assert(!succs.empty());
                StateID* ptr = &succs.back();

                for (int i = succs.size(); i > 0; --i, --ptr) {
                    const auto succ_id =
                        get_state_id(*ptr); // ptr->hash() - start;
                    succs.pop_back();
                    StateInfo& succ_info =
                        get_state_info[succ_id]; // infos[succ_id];
                    if (!succ_info.explored) {
                        s.successors.back().emplace_back(stack_.size());
                        if (pushf(succ_id, succ_info)) { // , scc[succ_id]
                            backtracked = false;
                            break;
                        } else {
                            assert(!succ_info.onstack());
                            e.recurse =
                                e.recurse ||
                                (e.flag && s.successors.back().size() > 1);
                            e.flag = false;
                            if (Store<ZeroStateIDSet>::value &&
                                succ_info.one()) {
                                e.dead = false;
                            }
                        }
                    } else if (succ_info.onstack()) {
                        e.recurse = e.recurse || !e.flag;
                        e.lstck = std::min(e.lstck, succ_info.stackid());
                        s.successors.back().emplace_back(succ_info.stackid());
                    } else {
                        e.recurse = e.recurse ||
                                    (e.flag && !s.successors.back().empty());
                        e.flag = false;
                        if (Store<ZeroStateIDSet>::value && !succ_info.zero()) {
                            e.dead = false;
                        }
                    }
                }

                if (backtracked) {
                    assert(succs.empty());
                    e.successors.pop_back();
                    if (e.flag) {
                        assert(!s.successors.back().empty());
                        s.successors.emplace_back();
                        s.aops.push_back(e.aops.back());
                    } else {
                        s.successors.back().clear();
                        e.flag = true;
                    }
                    e.aops.pop_back();
                } else {
                    break;
                }
            }

            if (backtracked) {
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
                    unsigned scc_size = stack_.size() - e.stck;
                    StackInfo* scc_elem = &stack_.back();
                    if (Store<ZeroStateIDSet>::value && e.dead) {
                        if (RootIteration) {
                            const bool singleton = (scc_size == 1);
                            stats_.sccs1 += singleton;
                            stats_.sccs1_dead += singleton;
                            stats_.sccsk += !singleton;
                            stats_.sccsk_dead += !singleton;
                        }
                        for (unsigned i = scc_size; i > 0; --i, --scc_elem) {
                            StateInfo& info = get_state_info[scc_elem->stateid];
                            info.stackid_ = StateInfo::ZERO;
                            insert(zero_states, scc_elem->stateid);
                        }
                        stack_.resize(e.stck);
                    } else {
                        StateID scc_repr_id = s.stateid;
                        if (scc_size == 1) {
                            assert(s.aops.empty());
                            StateInfo& info = get_state_info[scc_repr_id];
                            info.stackid_ = StateInfo::UNDEF;
                            if (Store<OneStateIDSet>::value) {
                                info.explored = 0;
                            }
                            stack_.pop_back();
                            ++stats_.ec1;
                            if (RootIteration) {
                                ++stats_.sccs1;
                            }
                        } else {
                            if (ExpandGoalStates) {
                                for (unsigned i = scc_size; i > 0;
                                     --i, --scc_elem) {
                                    assert(
                                        scc_elem->successors.size() ==
                                        scc_elem->aops.size());
                                    StateInfo& info =
                                        get_state_info[scc_elem->stateid];
                                    if (info.flag) {
                                        scc_elem->successors.clear();
                                        scc_elem->aops.clear();
                                        recurse = true;
                                    }
                                }
                                scc_elem = &stack_.back();
                            }
                            if (recurse) {
                                ++stats_.recursions;
                                if (RootIteration) {
                                    ++stats_.sccsk;
                                }
                                for (unsigned i = scc_size; i > 0;
                                     --i, --scc_elem) {
                                    assert(
                                        scc_elem->successors.size() ==
                                        scc_elem->aops.size());
                                    StateInfo& info =
                                        get_state_info[scc_elem->stateid];
                                    info.stackid_ = StateInfo::UNDEF;
                                    info.explored = 0;
                                }
                                decompose<Store<OneStateIDSet>::value>(e.stck);
                            } else {
                                unsigned transitions = 0;
                                for (unsigned i = scc_size; i > 0;
                                     --i, --scc_elem) {
                                    assert(
                                        scc_elem->successors.size() ==
                                        scc_elem->aops.size());
                                    StateInfo& info =
                                        get_state_info[scc_elem->stateid];
                                    info.stackid_ = StateInfo::UNDEF;
                                    if (Store<OneStateIDSet>::value) {
                                        info.explored = 0;
                                    }
                                    transitions += scc_elem->aops.size();
                                }
                                StackStateIDIterator begin(stack_.begin() + e.stck);
                                StackStateIDIterator end(stack_.end());
                                StackAopsIterator abegin(stack_.begin() + e.stck);
                                sys_->build_quotient(
                                    begin,
                                    end,
                                    scc_repr_id,
                                    abegin);
                                stack_.resize(e.stck);
                                ++stats_.eck;
                                stats_.ec_transitions += transitions;
                                if (RootIteration) {
                                    ++stats_.sccsk;
                                }
                            }
                        }
                        if (Store<OneStateIDSet>::value) {
                            collect_one_states<OneStateIDSet>(
                                one_states,
                                scc_repr_id);
                        }
                    }
                    assert(stack_.size() == e.stck);
                }

                expansion_queue_.pop_back();
            }
        }
    }

    template <bool ResetExplored = false>
    void decompose(const unsigned start)
    {
        const unsigned scc_size = stack_.size() - start;
        const unsigned limit = expansion_queue_.size();

        std::vector<StackInfo> scc(stack_.begin() + start, stack_.end());
        stack_.resize(start);

        PushLocal push_local(&scc, &expansion_queue_, &stack_, &stats_);
        StateInfoLookup get_state_info(&scc, &state_infos_);
        SuccIDToLocal get_succ_id(start);

        for (unsigned i = 0; i < scc_size; ++i) {
            StateInfo& iinfo = get_state_info[i];
            if (iinfo.explored) {
                continue;
            }
            push_local(i, iinfo);
            find_and_decompose_sccs<false, const void*, const void*>(
                limit,
                push_local,
                get_state_info,
                get_succ_id,
                nullptr,
                nullptr);
        }

        if (ResetExplored) {
            StackInfo* it = &scc[0];
            for (int i = scc_size; i > 0; --i, ++it) {
                state_infos_[it->stateid].explored = 0;
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
        Distribution<StateID> transition;
        q_aops_gen_->operator()(state_id, aops);
        assert(!aops.empty());
        expansion_queue_.emplace_back(stack1_.size());
        ExpansionInfo& e = expansion_queue_.back();
        e.dead = !state_info.flag;
        e.successors.resize(aops.size());
        const auto* aptr = &aops[0];
        std::vector<StateID>* succs = &e.successors[0];
        for (int i = aops.size(); i > 0; --i, ++aptr) {
            q_transition_gen_->operator()(state_id, *aptr, transition);
            for (auto it = transition.begin(); it != transition.end(); ++it) {
                const StateID succ_id = it->first;
                if (succ_id != state_id) {
                    succs->push_back(succ_id);
                }
            }
            if (!succs->empty()) {
                ++succs;
            }
            transition.clear();
        }
        e.successors.resize(succs - &e.successors[0]);
        assert(!e.successors.empty());
        state_info.stackid_ = stack1_.size();
        state_info.explored = 1;
        stack1_.emplace_back(state_id);
        if (ExpandGoalStates && state_info.flag) {
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
                    e.flag = e.flag && one;
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
                const StateID* succ = &succs.back();
                for (int i = succs.size(); i > 0; --i, --succ) {
                    StateID succ_id = *succ;
                    succs.pop_back();
                    StateInfo& succ_info = state_infos_[succ_id];
                    if (!succ_info.explored) {
                        push1(succ_id, succ_info);
                        backtracked = false;
                        break;
                    } else if (succ_info.onstack()) {
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
                if (backtracked) {
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
                } else {
                    break;
                }
            }
            if (backtracked) {
                one = s.one;
                lstck = e.lstck;
                backtracked_from = &s;
                onstack = e.stck != e.lstck;
                if (!onstack) {
                    const unsigned scc_size = stack1_.size() - e.stck;
                    std::deque<StackInfo1*> non_one;
                    {
                        StackInfo1* scc_elem = &stack1_.back();
                        for (int i = scc_size; i > 0; --i, --scc_elem) {
                            if (!scc_elem->one && !scc_elem->scc_transitions &&
                                !scc_elem->parents.empty()) {
                                non_one.push_back(scc_elem);
                            }
                        }
                    }
                    while (!non_one.empty()) {
                        StackInfo1* scc_elem = non_one.back();
                        non_one.pop_back();
                        std::pair<unsigned, unsigned>* parent =
                            &scc_elem->parents[0];
                        for (int i = scc_elem->parents.size(); i > 0;
                             --i, ++parent) {
                            StackInfo1& pinfo = stack1_[parent->first];
                            if (!pinfo.one && pinfo.active[parent->second]) {
                                pinfo.active[parent->second] = false;
                                if (--pinfo.scc_transitions == 0 &&
                                    !pinfo.parents.empty()) {
                                    non_one.push_back(&pinfo);
                                }
                            }
                        }
                    }
                    StackInfo1* scc_elem = &stack1_.back();
                    for (int i = scc_size; i > 0; --i, --scc_elem) {
                        auto it = sys_->quotient_range(scc_elem->stateid);
                        if (scc_elem->one || scc_elem->scc_transitions) {
                            for (; it.b != it.e; ++it.b) {
                                StateInfo& sinfo = state_infos_[*it.b];
                                sinfo.explored = 1;
                                sinfo.stackid_ = StateInfo::ONE;
                                insert(one_states, *it.b);
                                ++stats_.ones;
                            }
                        } else {
                            for (; it.b != it.e; ++it.b) {
                                StateInfo& sinfo = state_infos_[*it.b];
                                sinfo.explored = 1;
                                sinfo.stackid_ = StateInfo::UNDEF;
                            }
                        }
                    }
                    stack1_.resize(e.stck);
                }
                expansion_queue_.pop_back();
            }
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