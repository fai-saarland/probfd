#ifndef PROBFD_TRANSITION_GENERATOR_H
#define PROBFD_TRANSITION_GENERATOR_H

#include "probfd/engine_interfaces/state_space.h"

#include "probfd/storage/per_state_storage.h"
#include "probfd/storage/segmented_memory_pool.h"

#include "probfd/task_proxy.h"

#include "algorithms/segmented_vector.h"

#include "task_utils/successor_generator.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <vector>

class State;
class StateRegistry;
class Evaluator;

namespace probfd {
class ProbabilisticTask;

namespace engine_interfaces {

template <>
class StateSpace<State, OperatorID> {
protected:
    struct Statistics {
        unsigned long long single_transition_generator_calls = 0;
        unsigned long long all_transitions_generator_calls = 0;
        unsigned long long aops_generator_calls = 0;

        unsigned long long generated_operators = 0;
        unsigned long long generated_states = 0;

        unsigned long long aops_computations = 0;
        unsigned long long computed_operators = 0;

        unsigned long long transition_computations = 0;
        unsigned long long computed_successors = 0;

        void print(std::ostream& out) const;
    };

    struct CacheEntry {
        bool is_initialized() const
        {
            return naops != std::numeric_limits<unsigned>::max();
        }

        unsigned naops = std::numeric_limits<unsigned>::max();
        OperatorID* aops = nullptr;
        StateID* succs = nullptr;
    };

    using Cache = storage::PerStateStorage<CacheEntry>;

protected:
    std::shared_ptr<ProbabilisticTask> task;
    ProbabilisticTaskProxy task_proxy;

    successor_generator::SuccessorGenerator gen_;
    StateRegistry* state_registry_;

    const bool caching_;
    const std::vector<std::shared_ptr<::Evaluator>> notify_;

    Cache cache_;
    storage::SegmentedMemoryPool<> cache_data_;

    std::vector<OperatorID> aops_;
    std::vector<StateID> successors_;

    Statistics statistics_;

public:
    StateSpace(
        std::shared_ptr<ProbabilisticTask> task,
        StateRegistry* state_registry,
        const std::vector<std::shared_ptr<::Evaluator>>&
            path_dependent_evaluators,
        bool enable_caching);

    StateID get_state_id(const State& state);
    State get_state(StateID state_id);

    ActionID get_action_id(StateID, OperatorID op_id);
    OperatorID get_action(StateID, ActionID action_id);

    void generate_applicable_actions(
        StateID state_id,
        std::vector<OperatorID>& result);

    void generate_action_transitions(
        StateID state,
        OperatorID operator_id,
        Distribution<StateID>& result);

    void generate_all_transitions(
        StateID state,
        std::vector<OperatorID>& aops,
        std::vector<Distribution<StateID>>& successors);

    void print_statistics(std::ostream& out) const;

protected:
    void compute_successor_states(
        const State& s,
        OperatorID op_id,
        std::vector<ItemProbabilityPair<StateID>>& successors);

    void
    compute_applicable_operators(const State& s, std::vector<OperatorID>& ops);

    bool setup_cache(StateID state_id, CacheEntry& entry);

    CacheEntry& lookup(StateID state_id);
    CacheEntry& lookup(StateID state_id, bool& initialized);
};

} // namespace engine_interfaces

} // namespace probfd

#endif // __TRANSITION_GENERATOR_H__