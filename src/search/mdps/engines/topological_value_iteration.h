#ifndef MDPS_ENGINES_TOPOLOGICAL_VALUE_ITERATION_H
#define MDPS_ENGINES_TOPOLOGICAL_VALUE_ITERATION_H

#include "../storage/per_state_storage.h"
#include "../value_utils.h"
#include "engine.h"

#include <deque>
#include <iostream>
#include <memory>
#include <type_traits>
#include <vector>

namespace probabilistic {
namespace engines {

/// Namespace dedicated to Topological Value Iteration (TVI).
namespace topological_vi {

/**
 * @brief Topological value iteration statistics.
 */
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

inline bool bounds_equal(const value_type::value_t&)
{
    return true;
}

inline bool bounds_equal(const value_utils::IntervalValue& v)
{
    return value_type::approx_equal()(v.lower, v.upper);
}

template <typename T>
using ValueStore =
    storage::PersistentPerStateStorage<value_utils::IncumbentSolution<T>>;

inline bool contains(void*, const StateID&)
{
    return false;
}

inline bool contains(storage::PerStateStorage<bool>* store, const StateID& s)
{
    return store->operator[](s);
}

/**
 * @brief Implements topological value iteration \cite dai:etal:jair-11 .
 *
 * @tparam State - The state type of the underlying MDP model.
 * @tparam Action - The action type of the underlying MDP model.
 * @tparam ExpandGoals - Whether the algorithm should expand goal states
 * or treat them as terminal.
 * @tparam Interval - Whether bounded value iteration is used.
 * @tparam ZeroStates - Storage type for dead-end states??
 * @tparam OneStates - Storage type for probability one states??
 */
template <
    typename State,
    typename Action,
    bool ExpandGoals = false,
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
        const StateEvaluator<State>* value_initializer)
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
        , dead_end_value_(this->get_minimal_reward())
        , state_information_()
        , value_store_(nullptr)
        , index_(0)
        , exploration_stack_()
        , stack_()
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
        return value_utils::as_upper_bound(
            value_store_->operator[](this->get_state_id(s)));
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        statistics_.print(out);
    }

    Statistics get_statistics() const { return statistics_; }

    template <typename VS, typename BoolStore = NoStore>
    value_type::value_t solve(
        const State& initial_state,
        VS& value_store,
        BoolStore* dead_end_store = nullptr)
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
            , dfs_index(0)
            , lowlink(0)
        {
        }

        unsigned status : 31;
        unsigned dead : 1;
        unsigned dfs_index;
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

        const Action*
        next_action(TopologicalValueIteration* base, unsigned int state_id)
        {
            while (!aops.empty()) {
                transition.clear();
                const Action& action = aops.back();
                base->generate_successors(state_id, action, transition);
                transition.make_unique();
                successor = transition.begin();

                if (transition.size() != 1 || successor->first != state_id) {
                    return &action;
                }

                aops.pop_back();
            }

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
        template <typename T>
        BellmanBackupInfo(T base)
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
                    base *= self_loop_prob;
                }
            }

            return successors.empty();
        }

        ValueT operator()() const
        {
            ValueT res = base;

            for (auto& [prob, value] : successors) {
                res += prob * (*value);
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
                value_utils::set_max(val, info());
            }

            return value_utils::update(*value, val) || !bounds_equal(*value);
        }

        StateID state_id;
        ValueT* value;
        ValueT b;
        std::vector<BellmanBackupInfo> infos;
    };

    template <typename ValueStore, typename BoolStore>
    bool push_state(
        StateID state_id,
        StateInfo& state_info,
        ValueStore& value_store,
        BoolStore* dead_end_store)
    {
        assert(state_info.status == StateInfo::NEW);

        state_info.dfs_index = state_info.lowlink = index_++;
        state_info.status = StateInfo::ONSTACK;

        ValueT& state_value = value_store[state_id];
        State state = this->lookup_state(state_id);
        EvaluationResult state_eval = this->get_state_reward(state);
        std::vector<Action> aops;

        if (state_eval) {
            state_value = ValueT(static_cast<value_type::value_t>(state_eval));
            state_info.dead = false;
            ++statistics_.goal_states;

            if constexpr (ExpandGoals) {
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
                        state_value.lower = this->get_minimal_reward();
                        state_value.upper =
                            static_cast<value_type::value_t>(estimate);
                    } else {
                        state_value =
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

            return false;
        }

        const auto state_reward = static_cast<value_type::value_t>(state_eval);

        ExplorationInfo& einfo = exploration_stack_.emplace_back(
            state_id,
            state_reward,
            stack_.size(),
            std::move(aops));

        const Action* action = einfo.next_action(this, state_id);

        if (!action) {
            exploration_stack_.pop_back();

            if (!ExpandGoals || state_info.status != StateInfo::TERMINAL) {
                ++statistics_.dead_ends;
                state_value = dead_end_value_;

                if (dead_end_store) {
                    dead_end_store->operator[](state_id) = true;
                }
            }

            state_info.status = StateInfo::CLOSED;

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

    template <typename ValueStore, typename BoolStore>
    value_type::value_t value_iteration(
        const State& initial_state,
        ValueStore& value_store,
        BoolStore* dead_end_store)
    {
        const StateID init_state_id = this->get_state_id(initial_state);
        StateInfo& iinfo = state_information_[init_state_id];

        if (iinfo.status == StateInfo::CLOSED) {
            return value_utils::as_upper_bound(value_store[init_state_id]);
        }

        push_state(init_state_id, iinfo, value_store, dead_end_store);

        StateInfo* backtracked_state_info_ = nullptr;

        while (!exploration_stack_.empty()) {
            ExplorationInfo& explore = exploration_stack_.back();

            const unsigned int state = explore.state;

            StackInfo& stack_info = stack_[explore.stackidx];
            StateInfo& state_info = state_information_[state];

            assert(state == stack_info.state_id);
            assert(
                (state_info.status == StateInfo::ONSTACK) ||
                (ExpandGoals && state_info.status == StateInfo::TERMINAL));

            if (backtracked_state_info_ != nullptr) {
                state_info.lowlink = std::min(
                    state_info.lowlink,
                    backtracked_state_info_->lowlink);
                state_info.dead &= backtracked_state_info_->dead;
                backtracked_state_info_ = nullptr;
            }

            bool recurse = successor_loop(
                explore,
                stack_info,
                state_info,
                state,
                value_store,
                dead_end_store);

            if (recurse) {
                continue;
            }

            // Check if an SCC was found.
            if (state_info.dfs_index == state_info.lowlink) {
                scc_found(explore, stack_info, state_info, dead_end_store);
            }

            backtracked_state_info_ = &state_info;
            exploration_stack_.pop_back();
        }

        return value_utils::as_upper_bound(value_store[init_state_id]);
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

            for (; explore.successor != explore.transition.end();
                 ++explore.successor) {
                const auto [succ_id, prob] = *explore.successor;

                if (succ_id == state) {
                    tinfo.has_self_loop = true;
                    tinfo.self_loop_prob += prob;
                    continue;
                }

                StateInfo& succ_info = state_information_[succ_id];
                int status = succ_info.status;

                if (status == StateInfo::ONSTACK) {
                    tinfo.successors.emplace_back(prob, &value_store[succ_id]);
                    state_info.lowlink =
                        std::min(state_info.lowlink, succ_info.dfs_index);
                } else {
                    if (status == StateInfo::NEW) {
                        if (push_state(
                                succ_id,
                                succ_info,
                                value_store,
                                dead_end_store)) {
                            return true; // recursion on new state
                        }
                    } else if (ExpandGoals && status == StateInfo::TERMINAL) {
                        state_info.lowlink =
                            std::min(state_info.lowlink, succ_info.dfs_index);
                    }

                    tinfo.base += prob * value_store[succ_id];
                    state_info.dead = state_info.dead && succ_info.dead;
                }
            }

            if (tinfo.finalize()) {
                if (!ExpandGoals || state_info.status != StateInfo::TERMINAL) {
                    value_utils::set_max(stack_info.b, tinfo.base);
                }

                stack_info.infos.pop_back();
            }

            const Action* next_action = explore.next_action(this, state);

            if (!next_action) {
                return false;
            }

            explore.aops.pop_back();

            const auto action_reward = get_action_reward(state, *next_action);
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
        assert(explore.stackidx < stack_.size());

        ++statistics_.sccs;

        const unsigned scc_size = stack_.size() - explore.stackidx;

        if (scc_size == 1) {
            ++statistics_.singleton_sccs;
        }

        if (state_info.dead) {
            auto it = stack_.begin() + explore.stackidx;
            auto end = stack_.end();

            do {
                StackInfo& stack_it = *it++;
                StateInfo& state_it = state_information_[stack_it.state_id];

                assert(state_it.dead);
                assert(state_it.status == StateInfo::ONSTACK);

                *stack_it.value = dead_end_value_;

                if (dead_end_store) {
                    dead_end_store->operator[](stack_it.state_id) = true;
                }

                state_it.status = StateInfo::CLOSED;
            } while (it != end);

            statistics_.dead_ends += scc_size;
        } else if (scc_size == 1) {
            assert(
                state_info.status == StateInfo::ONSTACK ||
                state_info.status == StateInfo::TERMINAL);

            if (!ExpandGoals || state_info.status == StateInfo::ONSTACK) {
                value_utils::update(*stack_info.value, stack_info.b);
            }

            state_info.status = StateInfo::CLOSED;
        } else {
            auto swap_it = stack_.begin() + explore.stackidx;
            auto end = stack_.end();

            // First loop iterates until a stack obbject to remove is found
            do {
                StackInfo& stack_it = *swap_it;
                StateInfo& state_it = state_information_[stack_it.state_id];

                assert(
                    state_it.status == StateInfo::ONSTACK ||
                    (ExpandGoals && state_it.status == StateInfo::TERMINAL));
                assert(ExpandGoals || !stack_it.infos.empty());

                if constexpr (ExpandGoals) {
                    if (state_it.status == StateInfo::TERMINAL) {
                        break;
                    } else if (stack_it.infos.empty()) {
                        value_utils::update(*stack_it.value, stack_it.b);
                        break;
                    }
                }

                state_it.status = StateInfo::CLOSED;
                state_it.dead = 0;
            } while (++swap_it != end);

            // If there is something to remove, shift elements that remain to
            // the beginning, i.e. copy std::remove_if
            if (swap_it != end) {
                assert(ExpandGoals);

                for (auto it = swap_it; ++it != end;) {
                    StackInfo& stack_it = *it;
                    StateInfo& state_it = state_information_[stack_it.state_id];

                    if (state_it.status != StateInfo::TERMINAL) {
                        assert(state_it.status == StateInfo::ONSTACK);
                        if (stack_it.infos.empty()) {
                            value_utils::update(*stack_it.value, stack_it.b);
                        } else {
                            *swap_it++ = std::move(*it);
                        }
                    }

                    state_it.status = StateInfo::CLOSED;
                    state_it.dead = 0;
                }

                stack_.erase(swap_it, end);
            }

            if (stack_.size() > explore.stackidx) {
                const auto begin = stack_.begin() + explore.stackidx;
                const unsigned n = bellman_backup(begin, stack_.end());
                statistics_.bellman_backups += n;
            }
        }

        stack_.erase(stack_.begin() + explore.stackidx, stack_.end());
    }

    template <typename StackInfoIterator>
    unsigned
    bellman_backup(StackInfoIterator begin, StackInfoIterator end) const
    {
        bool changed;
        unsigned num_updates = 0;

        do {
            changed = false;

            for (auto it = begin; it != end; ++it, ++num_updates) {
                changed |= it->update_value();
            }
        } while (changed);

        return num_updates;
    }

    const StateEvaluator<State>* value_initializer_;

    const ValueT dead_end_value_;

    storage::PerStateStorage<StateInfo> state_information_;
    std::unique_ptr<Store> value_store_;

    unsigned index_;
    std::deque<ExplorationInfo> exploration_stack_;
    std::vector<StackInfo> stack_;

    Statistics statistics_;
};

} // namespace topological_vi
} // namespace engines
} // namespace probabilistic

#endif // __TOPOLOGICAL_VALUE_ITERATION_H__