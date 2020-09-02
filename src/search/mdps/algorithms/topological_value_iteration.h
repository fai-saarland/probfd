#pragma once

#include "../analysis_result.h"
#include "../interfaces/i_engine.h"
#include "../interfaces/i_printable.h"
#include "types_common.h"
#include "types_storage.h"
#include "value_utils.h"

#include <deque>
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>

namespace probabilistic {
namespace algorithms {
namespace topological_vi {

namespace internal {

struct Statistics : public IPrintable {

    virtual void print(std::ostream& out) const override
    {
        out << "**** Topological Value Iteration ****" << std::endl;
        out << "Expanded state(s): " << expanded_states << std::endl;
        out << "Terminal state(s): " << terminal_states << std::endl;
        out << "Goal state(s): " << goal_states << std::endl;
        out << "Dead end state(s): " << dead_ends << std::endl;
        out << "Pruned state(s): " << pruned << std::endl;
        out << "Generated state(s): " << state_generations << std::endl;
        out << "Generated transition(s): " << transition_generations
            << std::endl;
        out << "Maximal SCCs: " << sccs << " (" << singleton_sccs
            << " are singleton)" << std::endl;
        out << "Bellman backups: " << bellman_backups << std::endl;
        if (print_error) {
            out << "Final error: " << error << std::endl;
        }
    }

    unsigned long long expanded_states = 0;
    unsigned long long terminal_states = 0;
    unsigned long long goal_states = 0;
    unsigned long long dead_ends = 0;
    unsigned long long state_generations = 0;
    unsigned long long transition_generations = 0;
    unsigned long long sccs = 0;
    unsigned long long singleton_sccs = 0;
    unsigned long long bellman_backups = 0;
    unsigned long long pruned = 0;

    bool print_error = false;
    value_type::value_t error = value_type::zero;
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
        if (vals.first > value) {
            value = vals.first;
        }
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
        if (val.first > value) {
            value = val.first;
        }
        if (val.second < value2) {
            value2 = val.second;
        }
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

} // namespace internal

template<
    typename State,
    typename Action,
    bool ExpandGoalStates = false,
    typename Interval = std::false_type,
    typename ZeroStates = void,
    typename OneStates = void>
class TopologicalValueIteration : public IMDPEngine<State> {
public:
    using ValueT = value_utils::IncumbentSolution<Interval>;
    using StateValueBounds = internal::StateValueBounds<Interval>;
    using ValueStore = internal::ValueStore<Interval>;

    TopologicalValueIteration(
        StateIDMap<State>* id_map,
        ApplicableActionsGenerator<State, Action>* aop_gen,
        TransitionGenerator<State, Action>* transition_gen,
        StateEvaluationFunction<State>* state_reward,
        TransitionRewardFunction<State, Action>* transition_reward,
        value_type::value_t dead_end_value,
        value_type::value_t ub,
        StateEvaluationFunction<State>* pruning_function,
        ZeroStates* zero_states = nullptr,
        OneStates* one_states = nullptr,
        value_type::value_t one_state_reward = value_type::zero)
        : state_id_map_(id_map)
        , aops_gen_(aop_gen)
        , transition_gen_(transition_gen)
        , state_reward_(state_reward)
        , transition_reward_(transition_reward)
        , prune_(pruning_function)
        , dead_end_value_(dead_end_value)
        , upper_bound_(ub)
        , zero_states_(zero_states)
        , one_states_(one_states)
        , one_state_reward_(one_state_reward)
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

    virtual AnalysisResult solve(const State& initial_state) override
    {
        void* ptr;
        return this->wrapper<void*, NoStore>(initial_state, ptr, nullptr);
    }

    template<typename VS>
    AnalysisResult solve(const State& initial_state, VS& value_store)
    {
        return this->wrapper<VS, NoStore>(initial_state, value_store, nullptr);
    }

    template<typename VS, typename BoolStore>
    AnalysisResult
    solve(const State& initial_state, VS& value_store, BoolStore& is_dead_end)
    {
        return this->wrapper<VS, BoolStore>(
            initial_state, value_store, &is_dead_end);
    }

private:
    // using value_t = value_type::value_t;

    struct NoStore {
        bool& operator[](const StateID&) const;
    };

    struct StateInfo {
        static constexpr const unsigned NEW = 0;
        static constexpr const unsigned OPEN = 1;
        static constexpr const unsigned ONSTACK = 2;
        static constexpr const unsigned CLOSED = 4;
        static constexpr const unsigned TERMINAL = 5;

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
        using transition_t = Distribution<State>;

        ExplorationInfo(
            const State& s,
            value_type::value_t reward,
            unsigned stackidx,
            Distribution<State>&& t,
            std::vector<Action>&& aops)
            : state(s)
            , state_reward(reward)
            , stackidx(stackidx)
            , aops(std::move(aops))
            , transition(std::move(t))
            , successor(transition.begin())
        {
            this->aops.pop_back();
        }

        State state;
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
            const auto* it = &successors[0];
            const auto* end = it + successors.size();
            for (; it != end; ++it) {
                value_utils::add(res, it->first, *it->second);
            }
            if (has_self_loop) {
                value_utils::mult(res, self_loop_prob);
            }
            return res;
        }

        bool has_self_loop;
        value_type::value_t self_loop_prob;

        ValueT base;
        std::vector<std::pair<value_type::value_t, StateValueBounds*>>
            successors;
    };

    struct StackInfo {

        StackInfo(
            const StateID& state_id,
            StateValueBounds* value,
            unsigned num_aops)
            : state_id(state_id)
            , value(value)
            , b(value_type::zero)
        {
            infos.reserve(num_aops);
        }

        bool update_value()
        {
            ValueT val = b;
            auto* it = &infos[0];
            for (int i = infos.size() - 1; i >= 0; i--, ++it) {
                auto tval = (*it)();
                value_utils::update_incumbent(val, tval);
            }
            return value->update(val) || !value->bounds_equal();
        }

        StateID state_id;
        StateValueBounds* value;
        value_utils::IncumbentSolution<Interval> b;
        std::vector<BellmanBackupInfo> infos;
    };

    template<typename VS, typename BS>
    AnalysisResult wrapper(const State& s, VS& vs, BS* bs)
    {
        ValueStore& vals = internal::convert_vs(value_store_, vs);
        AnalysisResult res = this->value_iteration(s, vals, bs);
        internal::copy_vs(vs, vals);
        return res;
    }

    template<typename BoolStore>
    bool push_state(
        const StateID& state_id,
        const State& state,
        StateInfo& state_info,
        ValueStore& value_store,
        BoolStore* dead)
    {
        assert(state_info.status == StateInfo::NEW);
        // std::cout << "push state " << state << " with id " << state_id <<
        // std::endl;
        state_info.index = state_info.lowlink = index_++;
        state_info.status = StateInfo::ONSTACK;
        StateValueBounds& state_value = value_store[state_id];
        auto x = state_reward_->operator()(state);
        std::vector<Action> aops;
        // std::cout << ((bool) x) << " " << std::endl;
        if (x) {
            state_value.set((value_type::value_t)x, ((value_type::value_t)x));
            state_info.dead = false;
            ++statistics_.goal_states;
            if (ExpandGoalStates) {
                state_info.status = StateInfo::TERMINAL;
                aops_gen_->operator()(state, aops);
                ++statistics_.expanded_states;
            }
        } else if (internal::contains(one_states_, state_id)) {
            state_value.set(one_state_reward_, one_state_reward_);
            state_info.dead = false;
            if (ExpandGoalStates) {
                state_info.status = StateInfo::TERMINAL;
                aops_gen_->operator()(state, aops);
                ++statistics_.expanded_states;
            }
        } else if (
            internal::contains(zero_states_, state_id)
            || (prune_ && prune_->operator()(state))) {
            state_value.set(dead_end_value_, dead_end_value_);
            ++statistics_.pruned;
        } else {
            aops_gen_->operator()(state, aops);
            ++statistics_.expanded_states;
            if (aops.empty()) {
                state_value.set(dead_end_value_, dead_end_value_);
            } else {
                state_value.set(dead_end_value_, upper_bound_);
            }
        }
        if (aops.empty()) {
            ++statistics_.terminal_states;
            state_info.status = StateInfo::CLOSED;
            if (!ExpandGoalStates || state_info.dead) {
                ++statistics_.dead_ends;
                if (!std::is_same<BoolStore, NoStore>::value) {
                    dead->operator[](state_id) = true;
                }
            }
            backtracked_state_value_ = &state_value;
            return false;
        }
        statistics_.transition_generations += aops.size();
        stack_.emplace_back(state_id, &state_value, aops.size());
        stack_.back().infos.emplace_back(
            (value_type::value_t)x
            + transition_reward_->operator()(state, aops.back()));
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

        exploration_stack_.emplace_back(
            state,
            (value_type::value_t)x,
            stack_.size() - 1,
            transition_gen_->operator()(state, aops.back()),
            std::move(aops));

        return true;
    }

    template<typename BoolStore>
    AnalysisResult value_iteration(
        const State& initial_state,
        ValueStore& value_store,
        BoolStore* is_dead_end)
    {
        AnalysisResult result;
        {
            StateID istate_id = state_id_map_->operator[](initial_state);
            StateInfo& iinfo = state_information_[istate_id];
            if (iinfo.status == StateInfo::CLOSED) {
                result.result = value_store[istate_id];
                result.statistics = new internal::Statistics();
                return result;
            }
            // std::cout << "initial state = " << istate_id << std::endl;
            push_state(
                istate_id, initial_state, iinfo, value_store, is_dead_end);
        }
        backtracked_state_info_ = nullptr;
        while (!exploration_stack_.empty()) {
            ExplorationInfo& explore = exploration_stack_.back();
            StackInfo& stack_info = stack_[explore.stackidx];
            StateInfo& state_info = state_information_[stack_info.state_id];
            // std::cout << stack_info.state_id << " [" << state_info.status <<
            // "]"
            //           << std::endl;
            assert(
                state_id_map_->operator[](explore.state)
                == stack_info.state_id);
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
                bool recur = false;
                assert(!stack_info.infos.empty());
                BellmanBackupInfo& tinfo = stack_info.infos.back();
                for (; explore.successor != explore.transition.end();
                     ++explore.successor) {
                    StateID succ_id =
                        state_id_map_->operator[](explore.successor->first);
                    // std::cout << " " << succ_id << std::flush;
                    if (succ_id == stack_info.state_id) {
                        tinfo.has_self_loop = true;
                        tinfo.self_loop_prob += explore.successor->second;
                        continue;
                    }
                    StateInfo& succ_info = state_information_[succ_id];
                    if (succ_info.status == StateInfo::NEW) {
                        if (push_state(
                                succ_id,
                                explore.successor->first,
                                succ_info,
                                value_store,
                                is_dead_end)) {
                            recur = true;
                            // std::cout << "~";
                            break;
                        } else {
                            value_utils::add(
                                tinfo.base,
                                explore.successor->second,
                                *backtracked_state_value_);
                            state_info.dead = state_info.dead && succ_info.dead;
                        }
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
                if (recur) {
                    break;
                } else {
                    if (tinfo.finalize()) {
                        if ((!ExpandGoalStates
                             || state_info.status != StateInfo::TERMINAL)) {
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
                                    if (!std::is_same<BoolStore, NoStore>::
                                            value) {
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
                                    assert(
                                        scc_state_info.status
                                        == StateInfo::ONSTACK);
                                    scc_state_info.status = StateInfo::CLOSED;
                                    if (it == scc_start) {
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
                                             --i) {
                                            StackInfo& scc_stack_info =
                                                stack_[i];
                                            StateInfo& scc_state_info =
                                                state_information_
                                                    [scc_stack_info.state_id];
                                            assert(
                                                scc_state_info.status
                                                    == StateInfo::ONSTACK
                                                || scc_state_info.status
                                                    == StateInfo::TERMINAL);
                                            if (scc_state_info.status
                                                == StateInfo::TERMINAL) {
                                                --scc_size;
                                                stack_.erase(
                                                    stack_.begin() + i);
                                            } else if (scc_stack_info.infos
                                                           .empty()) {
                                                scc_stack_info.value->update(
                                                    scc_stack_info.b);
                                                --scc_size;
                                                stack_.erase(
                                                    stack_.begin() + i);
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
                                                state_information_
                                                    [it->state_id];
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
                    } else {
                        // std::cout << " (applying "
                        //           << "..."
                        //           << std::flush;
                        explore.transition = transition_gen_->operator()(
                            explore.state, explore.aops.back());
                        // for (auto it = explore.transition.begin();
                        //      it != explore.transition.end();
                        //      ++it) {
                        //     std::cout << " "
                        //               <<
                        //               state_id_map_->operator[](it->first);
                        // }
                        // std::cout << ")" << std::flush;
                        statistics_.state_generations +=
                            explore.transition.size();
                        explore.successor = explore.transition.begin();
                        if (explore.transition.size() == 1
                            && state_id_map_->operator[](
                                   explore.successor->first)
                                == stack_info.state_id) {
                            explore.aops.pop_back();
                            goto check_backtrack;
                        } else {
                            stack_info.infos.emplace_back(
                                explore.state_reward
                                + transition_reward_->operator()(
                                    explore.state, explore.aops.back()));
                            explore.aops.pop_back();
                        }
                    }
                }
            } while (true);
            // std::cout << std::endl;
        }
        {
            const StateValueBounds& vb =
                value_store[state_id_map_->operator[](initial_state)];
            result.result = vb;
            statistics_.print_error =
                std::is_same<Interval, std::true_type>::value;
            statistics_.error = vb.error();
            result.statistics = new internal::Statistics(statistics_);
        }
        return result;
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

    StateIDMap<State>* state_id_map_;
    ApplicableActionsGenerator<State, Action>* aops_gen_;
    TransitionGenerator<State, Action>* transition_gen_;
    StateEvaluationFunction<State>* state_reward_;
    TransitionRewardFunction<State, Action>* transition_reward_;
    StateEvaluationFunction<State>* prune_;
    const value_type::value_t dead_end_value_;
    const value_type::value_t upper_bound_;
    ZeroStates* zero_states_;
    OneStates* one_states_;
    const value_type::value_t one_state_reward_;

    storage::PerStateStorage<StateInfo> state_information_;
    std::unique_ptr<ValueStore> value_store_;

    unsigned index_;
    std::deque<ExplorationInfo> exploration_stack_;
    std::vector<StackInfo> stack_;

    StateInfo* backtracked_state_info_;
    StateValueBounds* backtracked_state_value_;

    internal::Statistics statistics_;
};

} // namespace topological_vi
} // namespace algorithms
} // namespace probabilistic
