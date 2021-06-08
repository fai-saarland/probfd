#pragma once

#include "../end_components/end_component_decomposition.h"
#include "../quotient_system/engine_interfaces.h"
#include "../quotient_system/quotient_system.h"
#include "../storage/per_state_storage.h"
#include "engine.h"
#include "topological_value_iteration.h"

namespace probabilistic {
namespace engines {
namespace interval_iteration {

using ValueStore = topological_vi::ValueStore<std::true_type>;
using BoolStore = storage::PerStateStorage<bool>;

template<typename T>
unsigned
copy_lower_bounds(
    T& target,
    ValueStore* vs,
    BoolStore* filter0,
    BoolStore* filter1)
{
    unsigned res = 0;
    for (unsigned i = 0; i < vs->size(); ++i) {
        const StateID stateid(i);
        if (!filter0->operator[](stateid)
            && (filter1 == nullptr || !filter1->operator[](stateid))) {
            target[stateid] = vs->operator[](stateid).lower;
            ++res;
        }
    }
    return res;
}

template<typename T>
unsigned
copy_upper_bounds(
    T& target,
    ValueStore* vs,
    BoolStore* filter0,
    BoolStore* filter1)
{
    unsigned res = 0;
    for (unsigned i = 0; i < vs->size(); ++i) {
        const StateID stateid(i);
        if (!filter0->operator[](stateid)
            && (filter1 == nullptr || !filter1->operator[](stateid))) {
            target[stateid] = vs->operator[](stateid).upper;
            ++res;
        }
    }
    return res;
}

inline value_type::value_t
upper_bound(const ValueStore::reference b)
{
    return b.upper;
}

inline value_type::value_t
lower_bound(const ValueStore::reference b)
{
    return b.lower;
}

template<typename State, typename Action, bool ExpandGoalStates = false>
class IntervalIteration : public MDPEngine<State, Action> {
public:
    using Decomposer = end_components::
        EndComponentDecomposition<State, Action, ExpandGoalStates>;
    using QuotientSystem = typename Decomposer::QuotientSystem;
    using QAction = typename QuotientSystem::QAction;
    template<typename S>
    using ValueIteration = topological_vi::TopologicalValueIteration<
        State,
        QAction,
        ExpandGoalStates,
        std::true_type,
        BoolStore,
        S>;
    using ValueStore = interval_iteration::ValueStore;
    using BoolStore = interval_iteration::BoolStore;

    template<typename... Args>
    explicit IntervalIteration(
        StateEvaluator<State>* prune,
        bool extract_probability_one_states,
        Args... args)
        : MDPEngine<State, Action>(args...)
        , lb_(this->get_minimal_reward())
        , ub_(this->get_maximal_reward())
        , prune_(prune)
        , extract_probability_one_states_(extract_probability_one_states)
        , probability_one_state_reward_(ub_)
    {
    }

    virtual void solve(const State& state) override
    {
        BoolStore dead(false);
        BoolStore one(false);
        QuotientSystem* sys;
        this->mysolve(state, &value_store_, &dead, &one, sys);
        delete (sys);
    }

    virtual value_type::value_t get_result(const State& s) override
    {
        const StateID state_id = this->get_state_id(s);
        return upper_bound(value_store_[state_id]);
    }

    virtual bool supports_error_bound() const override { return true; }

    virtual value_type::value_t get_error(const State& s) override
    {
        const StateID state_id = this->get_state_id(s);
        auto& val = value_store_[state_id];
        return upper_bound(val) - lower_bound(val);
    }

    virtual void print_statistics(std::ostream& out) const override
    {
        tvi_statistics_.print(out);
        ecd_statistics_.print(out);
    }

    value_type::value_t solve(
        const State& state,
        ValueStore* value_store,
        BoolStore* dead_ends,
        BoolStore* one_states)
    {
        QuotientSystem* sys;
        value_type::value_t x =
            this->mysolve(state, value_store, dead_ends, one_states, sys);
        for (auto qit = sys->begin(); qit != sys->end(); ++qit) {
            StateID repr_id = *qit;
            auto states_iterators = sys->quotient_iterator(repr_id);
            auto& sit = states_iterators.first;
            const auto& send = states_iterators.second;
            const StateID repr = *sit;
            ++sit;
            const auto value = value_store->operator[](repr);
            const bool dead = dead_ends->operator[](repr);
            const bool one =
                (one_states == nullptr ? false : one_states->operator[](repr));
            for (; sit != send; ++sit) {
                value_store->operator[](*sit) = value;
                dead_ends->operator[](*sit) = dead;
                if (one_states != nullptr) {
                    one_states->operator[](*sit) = one;
                }
            }
        }
        delete (sys);
        return x;
    }

private:
    value_type::value_t mysolve(
        const State& state,
        ValueStore* value_store,
        BoolStore* dead_ends,
        BoolStore* one_states,
        QuotientSystem*& sys)
    {
        assert(dead_ends != nullptr);
        Decomposer ec_decomposer(
            prune_,
            this->get_action_id_map(),
            this->get_state_id_map(),
            this->get_state_reward_function(),
            this->get_applicable_actions_generator(),
            this->get_transition_generator());
        if (extract_probability_one_states_) {
            sys = ec_decomposer
                      .template build_quotient_system<BoolStore&, BoolStore&>(
                          state, *dead_ends, *one_states);
        } else {
            sys = ec_decomposer.template build_quotient_system<BoolStore&>(
                state, *dead_ends);
        }
        ecd_statistics_ = ec_decomposer.get_statistics();
        ApplicableActionsGenerator<QAction> q_aops_gen(sys);
        TransitionGenerator<QAction> q_transition_gen(sys);
        QuotientActionRewardEvaluator<Action> q_action_reward(
            sys, this->get_action_reward_function());
        ActionIDMap<QAction> q_action_id_map(sys);
        if (extract_probability_one_states_) {
            assert(one_states != nullptr);
            ValueIteration<BoolStore> vi(
                value_utils::IntervalValue(value_type::zero, value_type::one),
                nullptr,
                dead_ends,
                one_states,
                this->get_state_id_map(),
                &q_action_id_map,
                this->get_state_reward_function(),
                &q_action_reward,
                lb_,
                ub_,
                &q_aops_gen,
                &q_transition_gen);
            value_type::value_t result = vi.solve(state, *value_store);
            tvi_statistics_ = vi.get_statistics();
            return result;
        } else {
            ValueIteration<void> vi(
                value_utils::IntervalValue(value_type::zero, value_type::one),
                nullptr,
                dead_ends,
                static_cast<void*>(nullptr),
                this->get_state_id_map(),
                &q_action_id_map,
                this->get_state_reward_function(),
                &q_action_reward,
                lb_,
                ub_,
                &q_aops_gen,
                &q_transition_gen);
            value_type::value_t result = vi.solve(state, *value_store);
            tvi_statistics_ = vi.get_statistics();
            return result;
        }
    }

    const value_type::value_t lb_;
    const value_type::value_t ub_;
    StateEvaluator<State>* prune_;
    const bool extract_probability_one_states_;
    const value_type::value_t probability_one_state_reward_;

    end_components::Statistics ecd_statistics_;
    topological_vi::Statistics tvi_statistics_;

    ValueStore value_store_;
};

} // namespace interval_iteration
} // namespace engines
} // namespace probabilistic
