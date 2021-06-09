#pragma once

#include "../storage/per_state_storage.h"
#include "engine.h"
#include "../value_utils.h"

#include <deque>
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>

namespace probabilistic {
namespace engines {
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

inline bool bounds_equal(const value_utils::SingleValue&) { 
    return true;
}

inline bool bounds_equal(const value_utils::IntervalValue& v)
{
    return value_type::approx_equal()(v.lower, v.upper);
}

template<typename T>
using ValueStore = storage::PersistentPerStateStorage<value_utils::IncumbentSolution<T>>;

inline bool contains(void*, const StateID&) {
    return false;
}

inline bool contains(storage::PerStateStorage<bool>* store, const StateID& s)
{
    return store->operator[](s);
}

template<
    typename State,
    typename Action,
    bool ExpandGoalStates = false,
    typename Interval = std::false_type,
    typename OneStates = void>
class TopologicalValueIteration : public MDPEngine<State, Action> {
    struct NoStore {
        bool dummy;
        bool& operator[](StateID) { return dummy; }
    };

public:
    using ValueT = value_utils::IncumbentSolution<Interval>;
    using Store = ValueStore<Interval>;

    explicit TopologicalValueIteration(
        StateIDMap<State>* state_id_map,
        ActionIDMap<Action>* action_id_map,
        StateRewardFunction<State>* state_reward_function,
        ActionRewardFunction<Action>* action_reward_function,
        value_type::value_t minimal_reward,
        value_type::value_t maximal_reward,
        ApplicableActionsGenerator<Action>* aops_generator,
        TransitionGenerator<Action>* transition_generator,
        StateEvaluator<State>* value_initializer,
        OneStates* one_states = static_cast<OneStates*>(nullptr))
        : MDPEngine<State, Action>(
            state_id_map,
            action_id_map,
            state_reward_function,
            action_reward_function,
            minimal_reward,
            maximal_reward,
            aops_generator,
            transition_generator)
        , value_initializer_(value_initializer)
        , one_states_(one_states)
        , dead_end_value_(this->get_minimal_reward())
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

    virtual void solve(const State& initial_state) override
    {
        this->value_store_.reset(new Store());
        this->solve(initial_state, *this->value_store_);
    }

    virtual value_type::value_t get_result(const State& s) override
    {
        return as_upper_bound(value_store_->operator[](this->get_state_id(s)));
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

    Statistics get_statistics() const { return statistics_; }

    template<typename VS, typename BoolStore = NoStore>
    value_type::value_t
    solve(const State& initial_state, VS& value_store, BoolStore* dead_end_store = nullptr)
    {
        return value_iteration(initial_state, value_store, dead_end_store);
    }

private:
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
            StateID s,
            value_type::value_t reward,
            unsigned stackidx,
            std::vector<Action> aops)
            : state(s)
            , state_reward(reward)
            , stackidx(stackidx)
            , aops(std::move(aops))
            , transition()
            , successor(transition.end())
        {
        }

        template <bool do_while = true>
        const Action* advance_non_loop_action(
            TopologicalValueIteration* base,
            unsigned int state_id)
        {
            if constexpr (!do_while) {
                if (aops.empty()) {
                    return nullptr;
                }
            }

            do {
                transition.clear();
                const Action& action = aops.back();
                base->generate_successors(state_id, action, transition);
                transition.make_unique();
                successor = transition.begin();

                if (transition.size() != 1 || successor->first != state_id) {
                    return &action;
                }

                aops.pop_back();
            } while (!aops.empty());

            return nullptr;
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
        BellmanBackupInfo(T base)
            : has_self_loop(false)
            , self_loop_prob(value_type::zero)
            , base(base)
        {
        }

        bool finalize()
        {
            if (has_self_loop) {
                self_loop_prob = (value_type::one / (value_type::one - self_loop_prob));

                if (successors.empty()) {
                    base *= self_loop_prob;
                }
            }

            return successors.empty();
        }

        ValueT operator()() const
        {
            ValueT res = base;

            for (auto& [prob, value] : successors) {
                res +=  prob * (*value);
            }

            if (has_self_loop) {
                res *= self_loop_prob;
            }

            return res;
        }

        bool has_self_loop;
        value_type::value_t self_loop_prob;

        ValueT base;
        std::vector<std::pair<value_type::value_t, ValueT*>> successors;
    };

    struct StackInfo {
        StackInfo(
            const StateID& state_id,
            ValueT* value_ref,
            ValueT b,
            unsigned num_aops)
            : state_id(state_id)
            , value(value_ref)
            , b(b)
        {
            infos.reserve(num_aops);
        }

        bool update_value()
        {
            ValueT val = b;

            for (const BellmanBackupInfo& info : infos) {
                value_utils::update(val, info());
            }

            return update_check(*value, val) || !bounds_equal(*value);
        }

        StateID state_id;
        ValueT* value;
        ValueT b;
        std::vector<BellmanBackupInfo> infos;
    };

    template<typename ValueStore, typename BoolStore>
    bool push_state(
        StateID state_id,
        StateInfo& state_info,
        ValueStore& value_store,
        BoolStore* dead_end_store)
    {
        assert(state_info.status == StateInfo::NEW);
        
        state_info.index = state_info.lowlink = index_++;
        state_info.status = StateInfo::ONSTACK;

        ValueT& state_value = value_store[state_id];
        State state = this->lookup_state(state_id);
        EvaluationResult state_eval = this->get_state_reward(state);
        std::vector<Action> aops;

        if (state_eval) {
            state_value = ValueT(static_cast<value_type::value_t>(state_eval));
            state_info.dead = false;
            ++statistics_.goal_states;

            if constexpr (ExpandGoalStates) {
                state_info.status = StateInfo::TERMINAL;
                this->generate_applicable_ops(state_id, aops);
                ++statistics_.expanded_states;
            }
        } else if (one_states_ && contains(one_states_, state_id)) {
            state_value = one_state_reward_;
            state_info.dead = false;

            if constexpr (ExpandGoalStates) {
                state_info.status = StateInfo::TERMINAL;
                this->generate_applicable_ops(state_id, aops);
                ++statistics_.expanded_states;
            }
        } else {
            auto estimate = value_initializer_->operator()(state);
            
            if (estimate) {
                state_value = dead_end_value_;
                ++statistics_.pruned;
            } else {
                this->generate_applicable_ops(state_id, aops);
                ++statistics_.expanded_states;

                if (aops.empty()) {
                    state_value = dead_end_value_;
                } else {
                    if constexpr (Interval::value) {
                        state_value.lower =
                            this->get_minimal_reward();
                        state_value.upper =
                            static_cast<value_type::value_t>(estimate);
                    } else {
                        state_value.value =
                            static_cast<value_type::value_t>(estimate);
                    }
                }
            }
        }

        if (aops.empty()) {
            ++statistics_.terminal_states;
            state_info.status = StateInfo::CLOSED;

            if (state_info.dead) {
                ++statistics_.dead_ends;
                if (dead_end_store) {
                    dead_end_store->operator[](state_id) = true;
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

        const auto state_reward = static_cast<value_type::value_t>(state_eval);

        ExplorationInfo& einfo = exploration_stack_.emplace_back(
            state_id, state_reward, stack_.size(), std::move(aops));

        const Action* action = einfo.advance_non_loop_action(this, state_id);

        if (!action) {
            exploration_stack_.pop_back();

            if (!ExpandGoalStates || state_info.status != StateInfo::TERMINAL) {
                ++statistics_.dead_ends;
                state_value = dead_end_value_;

                if (dead_end_store) {
                    dead_end_store->operator[](state_id) = true;
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
            einfo.aops.size());

        einfo.aops.pop_back();

        const auto action_reward = this->get_action_reward(state_id, *action);

        s_info.infos.emplace_back(state_reward + action_reward);

        return true;
    }

    template<typename ValueStore, typename BoolStore>
    value_type::value_t value_iteration(
        const State& initial_state,
        ValueStore& value_store,
        BoolStore* dead_end_store)
    {        
        const StateID init_state_id = this->get_state_id(initial_state);
        StateInfo& iinfo = state_information_[init_state_id];

        if (iinfo.status == StateInfo::CLOSED) {
            return as_upper_bound(value_store[init_state_id]);
        }

        push_state(init_state_id, iinfo, value_store, dead_end_store);

        backtracked_state_info_ = nullptr;

        while (!exploration_stack_.empty()) {
            ExplorationInfo& explore = exploration_stack_.back();

            const unsigned int state = explore.state;

            StackInfo& stack_info = stack_[explore.stackidx];
            StateInfo& state_info = state_information_[state];

            assert(state == stack_info.state_id);
            assert((state_info.status == StateInfo::ONSTACK) ||
                (ExpandGoalStates && state_info.status == StateInfo::TERMINAL));

            if (backtracked_state_info_ != nullptr) {
                state_info.lowlink = std::min(state_info.lowlink, backtracked_state_info_->lowlink);
                state_info.dead = state_info.dead && backtracked_state_info_->dead;
                backtracked_state_info_ = nullptr;
            }

            bool recurse = successor_loop(
                explore, stack_info, state_info, state, value_store, dead_end_store);

            if (recurse) {
                continue; 
            }

            // Check if an SCC was found.
            if (state_info.index == state_info.lowlink) {
                scc_found(explore, stack_info, state_info, dead_end_store);
            }

            backtracked_state_info_ = &state_info;
            exploration_stack_.pop_back();
        }

        return as_upper_bound(value_store[init_state_id]);
    }

    template <typename ValueStore, typename BoolStore>
    bool successor_loop(
        ExplorationInfo& explore,
        StackInfo& stack_info,
        StateInfo& state_info,
        const unsigned int state,
        ValueStore& value_store,
        BoolStore* dead_end_store)
    {
        const value_type::value_t state_reward = explore.state_reward;

        do {
            assert(!stack_info.infos.empty());

            BellmanBackupInfo& tinfo = stack_info.infos.back();

            for (; explore.successor != explore.transition.end(); ++explore.successor) 
            {
                const auto [succ_id, prob] = *explore.successor;

                if (succ_id == state) {
                    tinfo.has_self_loop = true;
                    tinfo.self_loop_prob += prob;
                    continue;
                }

                StateInfo& succ_info = state_information_[succ_id];

                if (succ_info.status == StateInfo::NEW) {
                    if (push_state(succ_id, succ_info, value_store, dead_end_store)) {
                        return true; // recursion on new state
                    } 

                    tinfo.base += prob * (*backtracked_state_value_);
                    state_info.dead = state_info.dead && succ_info.dead;
                } else if (succ_info.status == StateInfo::ONSTACK) {
                    tinfo.successors.emplace_back(prob, &value_store[succ_id]);
                    state_info.lowlink = std::min(state_info.lowlink, succ_info.index);
                } else {
                    if (ExpandGoalStates && succ_info.status == StateInfo::TERMINAL) {
                        state_info.lowlink = std::min(state_info.lowlink, succ_info.index);
                    }

                    tinfo.base += prob * value_store[succ_id];
                    state_info.dead = state_info.dead && succ_info.dead;
                }
            }

            if (tinfo.finalize()) {
                if (!ExpandGoalStates || state_info.status != StateInfo::TERMINAL) {
                    value_utils::update(stack_info.b, tinfo.base);
                }

                stack_info.infos.pop_back();
            }

            const Action* next_action =
                explore.template advance_non_loop_action<false>(this, state);

            if (!next_action) {
                return false;
            }

            explore.aops.pop_back();

            const auto action_reward = this->get_action_reward(state, *next_action);
            stack_info.infos.emplace_back(state_reward + action_reward);
        } while (true);
    }

    template <typename BoolStore>
    void scc_found(
        ExplorationInfo& explore,
        StackInfo& stack_info,
        StateInfo& state_info,
        BoolStore* dead_end_store)
    {
        ++statistics_.sccs;

        if (explore.stackidx + 1 == stack_.size()) {
            ++statistics_.singleton_sccs;
        }

        if (state_info.dead) {
            const StackInfo* scc_start = &stack_[explore.stackidx];
            StackInfo* it = &stack_.back();

            do {
                *it->value = dead_end_value_;

                if (dead_end_store) {
                    dead_end_store->operator[](it->state_id) = true;
                }

                StateInfo& scc_state_info = state_information_[it->state_id];
                    
                assert(scc_state_info.dead);
                assert(scc_state_info.status == StateInfo::ONSTACK);

                scc_state_info.status = StateInfo::CLOSED;

                if (scc_start == it) {
                    break;
                }

                --it;
            } while (true);

            statistics_.dead_ends += (stack_.size() - explore.stackidx);
        } else {
            assert(explore.stackidx < stack_.size());
            unsigned scc_size = stack_.size() - explore.stackidx;

            if (scc_size == 1) {
                assert(state_info.status == StateInfo::ONSTACK ||
                    state_info.status == StateInfo::TERMINAL);

                if (!ExpandGoalStates || state_info.status == StateInfo::ONSTACK) {
                    update(*stack_info.value, stack_info.b);
                }

                state_info.status = StateInfo::CLOSED;
            } else {
                if constexpr (ExpandGoalStates) {
                    const int scc_start = explore.stackidx;

                    for (int i = stack_.size() - 1; i >= scc_start; --i) {
                        StackInfo& scc_stack_info = stack_[i];
                        StateInfo& scc_state_info =
                            state_information_[scc_stack_info.state_id];

                        assert(scc_state_info.status == StateInfo::ONSTACK ||
                            scc_state_info.status == StateInfo::TERMINAL);

                        if (scc_state_info.status == StateInfo::TERMINAL) {
                            --scc_size;
                            stack_.erase(stack_.begin() + i);
                        } else if (scc_stack_info.infos.empty()) {
                            update(*scc_stack_info.value, scc_stack_info.b);
                            --scc_size;
                            stack_.erase(stack_.begin() + i);
                        }

                        scc_state_info.dead = 0;
                        scc_state_info.status = StateInfo::CLOSED;
                    }
                } else {
                    const StackInfo* scc_start = &stack_[explore.stackidx];
                    StackInfo* it = &stack_.back();

                    do {
                        StateInfo& scc_state_info = state_information_[it->state_id];

                        assert(scc_state_info.status == StateInfo::ONSTACK);
                        assert(!it->infos.empty());

                        scc_state_info.status = StateInfo::CLOSED;
                        scc_state_info.dead = 0;

                        if (scc_start == it) {
                            break;
                        }

                        --it;
                    } while (true);
                }

                if (scc_size > 0) {
                    const unsigned n = bellman_backup(stack_, explore.stackidx);
                    statistics_.bellman_backups += n;
                }
            }
        }

        while (stack_.size() > explore.stackidx) {
            stack_.pop_back();
        }
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

    StateEvaluator<State>* value_initializer_;

    OneStates* one_states_;
    
    const ValueT dead_end_value_;
    const ValueT one_state_reward_;

    storage::PerStateStorage<StateInfo> state_information_;
    std::unique_ptr<Store> value_store_;

    unsigned index_;
    std::deque<ExplorationInfo> exploration_stack_;
    std::vector<StackInfo> stack_;

    StateInfo* backtracked_state_info_;
    ValueT* backtracked_state_value_;

    Statistics statistics_;
};

} // namespace topological_vi
} // namespace engines
} // namespace probabilistic
