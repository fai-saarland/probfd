#pragma once

#include "../interfaces/i_engine.h"
#include "../interfaces/i_printable.h"
#include "end_component_decomposition.h"
#include "quotient_system.h"
#include "topological_value_iteration.h"
#include "types_common.h"
#include "types_storage.h"
#include "value_utils.h"

namespace probabilistic {
namespace algorithms {
namespace interval_iteration {

using ValueStore = topological_vi::internal::ValueStore<std::true_type>;
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
            target[stateid] = vs->operator[](stateid).value;
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
            target[stateid] = vs->operator[](stateid).value2;
            ++res;
        }
    }
    return res;
}

inline value_type::value_t
upper_bound(const ValueStore::reference b)
{
    return b.value2;
}

inline value_type::value_t
lower_bound(const ValueStore::reference b)
{
    return b.value;
}

template<typename State, typename Action, bool ExpandGoalStates = false>
class IntervalIteration : public IMDPEngine<State> {
public:
    using Decomposer = end_component_decomposition::
        EndComponentDecomposition<State, Action, ExpandGoalStates>;
    using QuotientSystem = typename Decomposer::QuotientSystem;
    using QState = typename QuotientSystem::QState;
    using QAction = typename QuotientSystem::QAction;
    template<typename S>
    using ValueIteration = topological_vi::TopologicalValueIteration<
        QState,
        QAction,
        ExpandGoalStates,
        std::true_type,
        BoolStore,
        S>;
    using ValueStore = interval_iteration::ValueStore;
    using BoolStore = interval_iteration::BoolStore;

    IntervalIteration(
        StateIDMap<State>* state_id_map,
        ApplicableActionsGenerator<State, Action>* aops_gen,
        TransitionGenerator<State, Action>* transition_gen,
        StateEvaluationFunction<State>* state_reward,
        TransitionRewardFunction<State, Action>* transition_reward,
        value_type::value_t lb,
        value_type::value_t ub,
        StateEvaluationFunction<State>* prune,
        bool one,
        value_type::value_t oner)
        : state_id_map_(state_id_map)
        , aops_gen_(aops_gen)
        , transition_gen_(transition_gen)
        , state_reward_(state_reward)
        , transition_reward_(transition_reward)
        , lb_(lb)
        , ub_(ub)
        , prune_(prune)
        , one_(one)
        , one_r_(oner)
    {
    }

    virtual AnalysisResult solve(const State& state) override
    {
        BoolStore dead(false);
        BoolStore one(false);
        ValueStore value_store;
        QuotientSystem* sys;
        AnalysisResult x = this->mysolve(state, &value_store, &dead, &one, sys);
        delete (sys);
        return x;
    }

    AnalysisResult solve(
        const State& state,
        ValueStore* value_store,
        BoolStore* dead_ends,
        BoolStore* one_states)
    {
        QuotientSystem* sys;
        AnalysisResult x =
            this->mysolve(state, value_store, dead_ends, one_states, sys);
        for (auto qit = sys->begin(); qit != sys->end(); ++qit) {
            auto sit = sys->quotient_begin(qit);
            const auto send = sys->quotient_end(qit);
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
    AnalysisResult mysolve(
        const State& state,
        ValueStore* value_store,
        BoolStore* dead_ends,
        BoolStore* one_states,
        QuotientSystem*& sys)
    {
        assert(dead_ends != nullptr);
        if (one_) {
            assert(one_states != nullptr);
            auto ec =
                Decomposer(
                    state_id_map_,
                    prune_,
                    state_reward_,
                    aops_gen_,
                    transition_gen_)
                    .template compute_quotient_system<BoolStore&, BoolStore&>(
                        state, *dead_ends, *one_states);
            quotient_system::QuotientSystemFunctionRegistry<State, Action>
                factory(ec.second);
            auto res =
                ValueIteration<BoolStore>(
                    factory.get_id_map(),
                    factory.get_aops_gen(),
                    factory.get_transition_gen(),
                    factory.create(state_reward_),
                    factory.create(transition_reward_),
                    lb_,
                    ub_,
                    factory.create(prune_),
                    dead_ends,
                    one_states,
                    one_r_)
                    .solve(ec.second->get_quotient_state(state), *value_store);
            res.statistics =
                new IPrintables({ (IPrintable*)ec.first, res.statistics });
            sys = ec.second;
            return res;
        } else {
            auto ec = Decomposer(
                          state_id_map_,
                          prune_,
                          state_reward_,
                          aops_gen_,
                          transition_gen_)
                          .template compute_quotient_system<BoolStore&>(
                              state, *dead_ends);
            quotient_system::QuotientSystemFunctionRegistry<State, Action>
                factory(ec.second);
            auto res =
                ValueIteration<void>(
                    factory.get_id_map(),
                    factory.get_aops_gen(),
                    factory.get_transition_gen(),
                    factory.create(state_reward_),
                    factory.create(transition_reward_),
                    lb_,
                    ub_,
                    factory.create(prune_),
                    dead_ends)
                    .solve(ec.second->get_quotient_state(state), *value_store);
            res.statistics =
                new IPrintables({ (IPrintable*)ec.first, res.statistics });
            sys = ec.second;
            return res;
        }
    }

    StateIDMap<State>* state_id_map_;
    ApplicableActionsGenerator<State, Action>* aops_gen_;
    TransitionGenerator<State, Action>* transition_gen_;
    StateEvaluationFunction<State>* state_reward_;
    TransitionRewardFunction<State, Action>* transition_reward_;
    value_type::value_t lb_;
    value_type::value_t ub_;
    StateEvaluationFunction<State>* prune_;
    bool one_;
    value_type::value_t one_r_;
};

} // namespace interval_iteration
} // namespace algorithms
} // namespace probabilistic
