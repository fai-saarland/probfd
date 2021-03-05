#pragma once

#include "../storage/per_state_storage.h"
#include "engine.h"
#include "value_utils.h"

#include <deque>
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>

namespace probabilistic {
namespace topological_vi {

struct Statistics {

    void print(std::ostream& out) const
    {
        out << "  Expanded state(s): " << expanded_states << std::endl;
        out << "  Terminal state(s): " << terminal_states << std::endl;
        out << "  Goal state(s): " << goal_states << std::endl;
        out << "  Dead end state(s): " << dead_ends << std::endl;
        out << "  Pruned state(s): " << pruned << std::endl;
        out << "  Maximal SCCs: " << sccs << " (" << singleton_sccs
            << " are singleton)" << std::endl;
        out << "  Bellman backups: " << bellman_backups << std::endl;
    }

    unsigned long long expanded_states = 0;
    unsigned long long terminal_states = 0;
    unsigned long long goal_states = 0;
    unsigned long long dead_ends = 0;
    unsigned long long sccs = 0;
    unsigned long long singleton_sccs = 0;
    unsigned long long bellman_backups = 0;
    unsigned long long pruned = 0;
};

template<typename T = std::false_type>
struct StateValueBounds {

    explicit StateValueBounds()
        : value(value_type::zero)
    {
    }

    void set(const value_type::value_t& lb, const value_type::value_t&)
    {
        value = lb;
    }

    bool update(const value_utils::IncumbentSolution<std::false_type>& vals)
    {
        const bool result = !value_type::approx_equal()(vals.first, value);
        value = vals.first;
        return result;
    }

    bool bounds_equal() const { return true; }

    value_type::value_t error() const { return value_type::zero; }

    operator value_type::value_t() const { return value; }

    value_type::value_t value;
};

template<>
struct StateValueBounds<std::true_type> {

    explicit StateValueBounds()
        : value(value_type::zero)
        , value2(value_type::zero)
    {
    }

    void set(const value_type::value_t& lb, const value_type::value_t& ub)
    {
        value = lb;
        value2 = ub;
    }

    bool update(const value_utils::IncumbentSolution<std::true_type>& val)
    {
        const bool result = !value_type::approx_equal()(val.first, value)
            || !value_type::approx_equal()(val.second, value2);
        value = val.first;
        value2 = val.second;
        assert(!value_type::approx_less()(value2, value));
        return result;
    }

    value_type::value_t error() const
    {
        return value_type::abs(value2 - value);
    }

    bool bounds_equal() const
    {
        return value_type::approx_equal()(value, value2);
    }

    operator value_type::value_t() const { return value; }

    value_type::value_t value;
    value_type::value_t value2;
};

template<typename T>
using ValueStore = storage::PersistentPerStateStorage<StateValueBounds<T>>;

template<typename T, typename VS>
inline ValueStore<T>&
convert_vs(std::unique_ptr<ValueStore<T>>& ptr, VS&)
{
    ptr = std::unique_ptr<ValueStore<T>>(new ValueStore<T>());
    return *ptr;
}

template<>
inline ValueStore<std::false_type>&
convert_vs(
    std::unique_ptr<ValueStore<std::false_type>>&,
    ValueStore<std::false_type>& vs)
{
    return vs;
}

template<>
inline ValueStore<std::true_type>&
convert_vs(
    std::unique_ptr<ValueStore<std::true_type>>&,
    ValueStore<std::true_type>& vs)
{
    return vs;
}

template<typename T, typename VS>
inline void
copy_vs(VS& result, ValueStore<T>& val_store)
{
    for (int i = val_store.size() - 1; i >= 0; --i) {
        const StateID stateid(i);
        result[stateid] = val_store[stateid];
    }
}

template<>
inline void
copy_vs(ValueStore<std::false_type>&, ValueStore<std::false_type>&)
{
}

template<>
inline void
copy_vs(ValueStore<std::true_type>&, ValueStore<std::true_type>&)
{
}

template<>
inline void
copy_vs(void*&, ValueStore<std::false_type>&)
{
}

template<>
inline void
copy_vs(void*&, ValueStore<std::true_type>&)
{
}

template<typename T>
inline bool contains(T*, const StateID&);

template<>
inline bool
contains(void*, const StateID&)
{
    return false;
}

template<>
inline bool
contains(storage::PerStateStorage<bool>* store, const StateID& s)
{
    return store->operator[](s);
}

template<
    typename State,
    typename Action,
    bool ExpandGoalStates = false,
    typename Interval = std::false_type,
    typename ZeroStates = void,
    typename OneStates = void>
class TopologicalValueIteration : public MDPEngine<State, Action> {
public:
    using ValueT = value_utils::IncumbentSolution<Interval>;
    using Bounds = StateValueBounds<Interval>;
    using Store = ValueStore<Interval>;

    template<typename... Args>
    explicit TopologicalValueIteration(
        StateEvaluator<State>* pruning_function,
        ZeroStates* zero_states,
        OneStates* one_states,
        Args... args)
        : MDPEngine<State, Action>(args...)
        , prune_(pruning_function)
        , dead_end_value_(this->get_minimal_reward())
        , upper_bound_(this->get_maximal_reward())
        , zero_states_(zero_states)
        , one_states_(one_states)
        , one_state_reward_(this->get_maximal_reward())
        , state_information_()
        , value_store_(nullptr)
        , index_(0)
        , exploration_stack_()
        , stack_()
        , backtracked_state_info_(nullptr)
        , backtracked_state_value_(nullptr)
        , statistics_()
    {
    }

    template<typename... Args>
    explicit TopologicalValueIteration(StateEvaluator<State>* pf, Args... args)
        : TopologicalValueIteration(
            pf,
            static_cast<ZeroStates*>(nullptr),
            static_cast<OneStates*>(nullptr),
            args...)
    {
    }

    virtual void solve(const State& initial_state) override
    {
        void* ptr;
        this->wrapper<void*, NoStore>(initial_state, ptr, nullptr);
    }

    virtual value_type::value_t get_result(const State& s) override
    {
        return value_type::value_t(
            value_store_->operator[](this->get_state_id(s)));
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

    Statistics get_statistics() const { return statistics_; }

    template<typename VS>
    value_type::value_t solve(const State& initial_state, VS& value_store)
    {
        return this->wrapper<VS, NoStore>(initial_state, value_store, nullptr);
    }

    template<typename VS, typename BoolStore>
    value_type::value_t
    solve(const State& initial_state, VS& value_store, BoolStore& is_dead_end)
    {
        return this->wrapper<VS, BoolStore>(
            initial_state, value_store, &is_dead_end);
    }

private:
    struct NoStore;

    struct StateInfo {
        static constexpr unsigned NEW = 0;
        static constexpr unsigned OPEN = 1;
        static constexpr unsigned ONSTACK = 2;
        static constexpr unsigned CLOSED = 4;
        static constexpr unsigned TERMINAL = 5;

        StateInfo()
            : status(NEW)
            , dead(true)
            , index(0)
            , lowlink(0)
        {
        }

        unsigned status : 31;
        unsigned dead : 1;
        unsigned index;
        unsigned lowlink;
    };

    struct ExplorationInfo {
        using transition_t = Distribution<StateID>;

        ExplorationInfo(
            const StateID& s,
            value_type::value_t reward,
            unsigned stackidx,
            std::vector<Action>&& aops)
            : state(s)
            , state_reward(reward)
            , stackidx(stackidx)
            , aops(std::move(aops))
            , transition()
            , successor(transition.end())
        {
        }

        StateID state;
        value_type::value_t state_reward;
        unsigned stackidx;

        std::vector<Action> aops;
        transition_t transition;
        typename transition_t::const_iterator successor;
    };

    struct BellmanBackupInfo {
        template<typename T>
        BellmanBackupInfo(const T base)
            : has_self_loop(false)
            , self_loop_prob(value_type::zero)
            , base(base)
        {
        }

        bool finalize()
        {
            if (has_self_loop) {
                self_loop_prob =
                    (value_type::one / (value_type::one - self_loop_prob));
                if (successors.empty()) {
                    value_utils::mult(base, self_loop_prob);
                }
            }
            return successors.empty();
        }

        ValueT operator()() const
        {
            ValueT res = base;
            for (auto& [prob, value] : successors) {
                value_utils::add(res, prob, *value);
            }
            if (has_self_loop) {
                value_utils::mult(res, self_loop_prob);
            }
            return res;
        }

        bool has_self_loop;
        value_type::value_t self_loop_prob;

        ValueT base;
        std::vector<std::pair<value_type::value_t, Bounds*>> successors;
    };

    struct StackInfo {
        StackInfo(
            const StateID& state_id,
            Bounds* value,
            const value_type::value_t& lb,
            const value_type::value_t& ,
            unsigned num_aops)
            : state_id(state_id)
            , value(value)
            , b(lb, lb)
        {
            infos.reserve(num_aops);
        }

        bool update_value()
        {
            ValueT val = b;
            for (const BellmanBackupInfo& info : infos) {
                value_utils::update_incumbent(val, info());
            }
            return value->update(val) || !value->bounds_equal();
        }

        StateID state_id;
        Bounds* value;
        ValueT b;
        std::vector<BellmanBackupInfo> infos;
    };

    template<typename VS, typename BS>
    value_type::value_t wrapper(const State& s, VS& vs, BS* bs)
    {
        Store& vals = convert_vs(value_store_, vs);
        value_type::value_t res = this->value_iteration(s, vals, bs);
        copy_vs(vs, vals);
        return res;
    }

    template<typename BoolStore>
    bool push_state(
        const StateID& state_id,
        StateInfo& state_info,
        Store& value_store,
        BoolStore* dead)
    {
        constexpr bool is_real_store = std::is_same_v<BoolStore, NoStore>;
        (void) dead; // silence warnings

        assert(state_info.status == StateInfo::NEW);
        // std::cout << "push state " << state_id <<
        // std::endl;
        state_info.index = state_info.lowlink = index_++;
        state_info.status = StateInfo::ONSTACK;
        Bounds& state_value = value_store[state_id];
        State state = this->lookup_state(state_id);
        auto x = this->get_state_reward(state);
        std::vector<Action> aops;
        // std::cout << ((bool) x) << " " << std::endl;

        if (x) {
            state_value.set((value_type::value_t)x, ((value_type::value_t)x));
            state_info.dead = false;
            ++statistics_.goal_states;
            if (ExpandGoalStates) {
                state_info.status = StateInfo::TERMINAL;
                this->generate_applicable_ops(state_id, aops);
                ++statistics_.expanded_states;
            }
        } else if (contains(one_states_, state_id)) {
            state_value.set(one_state_reward_, one_state_reward_);
            state_info.dead = false;
            if (ExpandGoalStates) {
                state_info.status = StateInfo::TERMINAL;
                this->generate_applicable_ops(state_id, aops);
                ++statistics_.expanded_states;
            }
        } else if (
            contains(zero_states_, state_id)
            || (prune_ && prune_->operator()(state))) {
            state_value.set(dead_end_value_, dead_end_value_);
            ++statistics_.pruned;
        } else {
            this->generate_applicable_ops(state_id, aops);
            ++statistics_.expanded_states;
            if (aops.empty()) {
                state_value.set(dead_end_value_, dead_end_value_);
            } else {
                state_value.set(value_type::zero, upper_bound_);
            }
        }

        // std::cout << state_value.value << std::endl;
        if (aops.empty()) {
            ++statistics_.terminal_states;
            state_info.status = StateInfo::CLOSED;
            if (state_info.dead) {
                ++statistics_.dead_ends;
                if constexpr (!is_real_store) {
                    dead->operator[](state_id) = true;
                }
            }
            backtracked_state_value_ = &state_value;
            return false;
        }
#if 0 
        exploration_stack_.emplace_back(
            state,
            (value_type::value_t)x,
            stack_.size() - 1,
            std::move(aops));
        exploration_stack_.back().transition = transition_gen_->operator()(state, exploration_stack_.back().aops.back());

        // std::cout << state_id << " " << aops.back()->get_name() << std::endl;
        // std::cout << "===> #" << exploration_stack_.back().transition.size() << ":";
        // for (auto it = exploration_stack_.back().transition.begin(); it != exploration_stack_.back().transition.end(); ++it) {
        //     std::cout << " " << state_id_map_->operator[](it->first);
        // }
        // std::cout << std::endl;

        exploration_stack_.back().successor = exploration_stack_.back().transition.begin();
        exploration_stack_.back().aops.pop_back();
#endif

        ExplorationInfo& einfo = exploration_stack_.emplace_back(
            state_id,
            (value_type::value_t)x,
            stack_.size(),
            std::move(aops));

        do {
            this->generate_successors(
                state_id, einfo.aops.back(), einfo.transition);
            einfo.transition.make_unique();
            einfo.successor = einfo.transition.begin();
            if (einfo.transition.size() != 1
                || einfo.successor->first != state_id)
            {
                break;
            }

            einfo.transition.clear();
            einfo.aops.pop_back();
        } while (!einfo.aops.empty());

        if (einfo.aops.empty()) {
            exploration_stack_.pop_back();
            if (!ExpandGoalStates || state_info.status != StateInfo::TERMINAL) {
                ++statistics_.dead_ends;
                state_value.set(dead_end_value_, dead_end_value_);
                if constexpr (!is_real_store) {
                    dead->operator[](state_id) = true;
                }
            }
            state_info.status = StateInfo::CLOSED;
            backtracked_state_value_ = &state_value;
            return false;
        }

        auto& s_info = stack_.emplace_back(
            state_id,
            &state_value,
            dead_end_value_,
            upper_bound_,
            einfo.aops.size());

        s_info.infos.emplace_back(
            (value_type::value_t)x
            + this->get_action_reward(state_id, einfo.aops.back()));
        // std::cout <<stack_.back().infos.back().base.first << std::endl;
        einfo.aops.pop_back();

        return true;
    }

    template<typename BoolStore>
    value_type::value_t value_iteration(
        const State& initial_state,
        Store& value_store,
        BoolStore* is_dead_end)
    {
        constexpr bool is_real_store = !std::is_same_v<BoolStore, NoStore>;
        (void) is_dead_end; // silence warnings

        {
            StateID istate_id = this->get_state_id(initial_state);
            StateInfo& iinfo = state_information_[istate_id];
            if (iinfo.status == StateInfo::CLOSED) {
                return value_store[istate_id];
            }
            // std::cout << "initial state = " << istate_id << std::endl;
            push_state(istate_id, iinfo, value_store, is_dead_end);
        }

        backtracked_state_info_ = nullptr;
        while (!exploration_stack_.empty()) {
            ExplorationInfo& explore = exploration_stack_.back();
            StackInfo& stack_info = stack_[explore.stackidx];
            StateInfo& state_info = state_information_[stack_info.state_id];
            // std::cout << stack_info.state_id << " [" << state_info.status <<
            // "]"
            //           << std::endl;
            assert(explore.state == stack_info.state_id);
            assert(
                state_info.status == StateInfo::ONSTACK
                || (ExpandGoalStates
                    && state_info.status == StateInfo::TERMINAL));

            if (backtracked_state_info_ != nullptr) {
                state_info.lowlink = std::min(
                    state_info.lowlink, backtracked_state_info_->lowlink);
                state_info.dead =
                    state_info.dead && backtracked_state_info_->dead;
                backtracked_state_info_ = nullptr;
            }

            do {
                assert(!stack_info.infos.empty());
                BellmanBackupInfo& tinfo = stack_info.infos.back();
                for (; explore.successor != explore.transition.end();
                     ++explore.successor) 
                {
                    const StateID succ_id = explore.successor->first;
                    // std::cout << " " << succ_id << std::flush;
                    if (succ_id == stack_info.state_id) {
                        tinfo.has_self_loop = true;
                        tinfo.self_loop_prob += explore.successor->second;
                        continue;
                    }

                    StateInfo& succ_info = state_information_[succ_id];
                    if (succ_info.status == StateInfo::NEW) {
                        if (push_state(
                                succ_id, succ_info, value_store, is_dead_end)) {
                            // std::cout << "~";
                            goto break_outer; // break outer loop
                        } 

                        value_utils::add(
                            tinfo.base,
                            explore.successor->second,
                            *backtracked_state_value_);
                        state_info.dead = state_info.dead && succ_info.dead;
                    } else if (succ_info.status == StateInfo::ONSTACK) {
                        tinfo.successors.emplace_back(
                            explore.successor->second, &value_store[succ_id]);
                        state_info.lowlink =
                            std::min(state_info.lowlink, succ_info.index);
                    } else {
                        if (ExpandGoalStates
                            && succ_info.status == StateInfo::TERMINAL) {
                            state_info.lowlink =
                                std::min(state_info.lowlink, succ_info.index);
                        }
                        value_utils::add(
                            tinfo.base,
                            explore.successor->second,
                            value_store[succ_id]);
                        state_info.dead = state_info.dead && succ_info.dead;
                    }
                }

                if (tinfo.finalize()) {
                    if (!ExpandGoalStates
                        || state_info.status != StateInfo::TERMINAL) {
                        value_utils::update_incumbent(
                            stack_info.b, tinfo.base);
                    }
                    stack_info.infos.pop_back();
                }
                
            check_backtrack:
                if (explore.aops.empty()) {
                    if (state_info.index == state_info.lowlink) {
                        ++statistics_.sccs;
                        statistics_.singleton_sccs +=
                            (explore.stackidx + 1 == stack_.size());
                        if (state_info.dead) {
                            const StackInfo* scc_start =
                                &stack_[explore.stackidx];
                            StackInfo* it = &stack_.back();
                            do {
                                it->value->set(
                                    dead_end_value_, dead_end_value_);

                                if constexpr (is_real_store) {
                                    is_dead_end->operator[](it->state_id) =
                                        true;
                                }

                                StateInfo& scc_state_info =
                                    state_information_[it->state_id];
                                // std::cout << it->state_id << ": "
                                //           << scc_state_info.dead << " "
                                //           << scc_state_info.status
                                //           << std::endl;
                                assert(scc_state_info.dead);
                                assert(scc_state_info.status
                                    == StateInfo::ONSTACK);
                                scc_state_info.status = StateInfo::CLOSED;

                                if (scc_start == it) {
                                    break;
                                }
                                --it;
                            } while (true);

                            statistics_.dead_ends +=
                                (stack_.size() - explore.stackidx);
                        } else {
                            assert(explore.stackidx < stack_.size());
                            unsigned scc_size =
                                stack_.size() - explore.stackidx;
                            if (scc_size == 1) {
                                assert(
                                    state_info.status == StateInfo::ONSTACK
                                    || state_info.status
                                        == StateInfo::TERMINAL);
                                if (!ExpandGoalStates
                                    || state_info.status
                                        == StateInfo::ONSTACK) {
                                    stack_info.value->update(stack_info.b);
                                }
                                state_info.status = StateInfo::CLOSED;
                            } else {
                                if (ExpandGoalStates) {
                                    const int scc_start = explore.stackidx;
                                    for (int i = stack_.size() - 1;
                                        i >= scc_start;
                                        --i)
                                    {
                                        StackInfo& scc_stack_info = stack_[i];
                                        StateInfo& scc_state_info =
                                            state_information_
                                                [scc_stack_info.state_id];
                                        assert(
                                            scc_state_info.status
                                                == StateInfo::ONSTACK
                                            || scc_state_info.status
                                                == StateInfo::TERMINAL);
                                        if (scc_state_info.status
                                            == StateInfo::TERMINAL)
                                        {
                                            --scc_size;
                                            stack_.erase(stack_.begin() + i);
                                        } else if (scc_stack_info.infos.empty())
                                        {
                                            scc_stack_info.value->update(
                                                scc_stack_info.b);
                                            --scc_size;
                                            stack_.erase(stack_.begin() + i);
                                        }
                                        scc_state_info.dead = 0;
                                        scc_state_info.status =
                                            StateInfo::CLOSED;
                                    }
                                } else {
                                    const StackInfo* scc_start =
                                        &stack_[explore.stackidx];
                                    StackInfo* it = &stack_.back();
                                    do {
                                        StateInfo& scc_state_info =
                                            state_information_[it->state_id];
                                        assert(
                                            scc_state_info.status
                                            == StateInfo::ONSTACK);
                                        assert(!it->infos.empty());
                                        scc_state_info.status =
                                            StateInfo::CLOSED;
                                        scc_state_info.dead = 0;
                                        if (scc_start == it) {
                                            break;
                                        }
                                        --it;
                                    } while (true);
                                }

                                if (scc_size > 0) {
                                    const unsigned n = bellman_backup(
                                        stack_, explore.stackidx);
                                    statistics_.bellman_backups += n;
                                }
                            }
                        }

                        while (stack_.size() > explore.stackidx) {
                            stack_.pop_back();
                        }
                        // std::cout << "[backtrack-from-scc]";
                    } // else std::cout << "[backtrack]";

                    backtracked_state_info_ = &state_info;
                    exploration_stack_.pop_back();
                    break;
                } 
                
                // std::cout << " (applying "
                //           << "..."
                //           << std::flush;
                explore.transition.clear();
                this->generate_successors(
                    explore.state,
                    explore.aops.back(),
                    explore.transition);
                explore.transition.make_unique();
                // for (auto it = explore.transition.begin();
                //      it != explore.transition.end();
                //      ++it) {
                //     std::cout << " "
                //               <<
                //               state_id_map_->operator[](it->first);
                // }
                // std::cout << ")" << std::flush;
                explore.successor = explore.transition.begin();
                if (explore.transition.size() == 1
                    && explore.successor->first == stack_info.state_id)
                {
                    explore.aops.pop_back();
                    goto check_backtrack; // oof
                }

                stack_info.infos.emplace_back(
                    explore.state_reward
                    + this->get_action_reward(
                        explore.state, explore.aops.back()));
                explore.aops.pop_back();
            } while (true);
        break_outer:;
            // std::cout << std::endl;
        }
        const Bounds& vb = value_store[this->get_state_id(initial_state)];
        return vb;
    }

    unsigned
    bellman_backup(std::vector<StackInfo>& stack, int start_index) const
    {
        assert(start_index < static_cast<int>(stack.size()));
        bool changed = true;
        unsigned num_updates = 0;
        while (changed) {
            changed = false;
            StackInfo* it = &stack[start_index];
            for (int i = stack.size() - start_index; i > 0; --i) {
                ++num_updates;
                changed = it->update_value() || changed;
                ++it;
            }
        }
        return num_updates;
    }

    StateEvaluator<State>* prune_;
    const value_type::value_t dead_end_value_;
    const value_type::value_t upper_bound_;
    ZeroStates* zero_states_;
    OneStates* one_states_;
    const value_type::value_t one_state_reward_;

    storage::PerStateStorage<StateInfo> state_information_;
    std::unique_ptr<Store> value_store_;

    unsigned index_;
    std::deque<ExplorationInfo> exploration_stack_;
    std::vector<StackInfo> stack_;

    StateInfo* backtracked_state_info_;
    Bounds* backtracked_state_value_;

    Statistics statistics_;
};

} // namespace topological_vi
} // namespace probabilistic
