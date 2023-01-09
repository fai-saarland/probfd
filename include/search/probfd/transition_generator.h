#ifndef MDPS_TRANSITION_GENERATOR_H
#define MDPS_TRANSITION_GENERATOR_H

#include "probfd/engine_interfaces/transition_generator.h"

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

namespace quotient_system {
template <typename T>
class QuotientSystem;
}

namespace engine_interfaces {

template <>
class TransitionGenerator<OperatorID> {
    friend class quotient_system::QuotientSystem<OperatorID>;

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
    const std::vector<std::shared_ptr<Evaluator>> notify_;

    Cache cache_;
    storage::SegmentedMemoryPool<> cache_data_;

    std::vector<OperatorID> aops_;
    std::vector<StateID> successors_;

    Statistics statistics_;

public:
    TransitionGenerator(
        std::shared_ptr<ProbabilisticTask> task,
        StateRegistry* state_registry,
        const std::vector<std::shared_ptr<Evaluator>>&
            path_dependent_evaluators,
        bool enable_caching);

    void generate_applicable_actions(
        const StateID& state_id,
        std::vector<OperatorID>& result);

    void generate_action_transitions(
        const StateID& state,
        OperatorID operator_id,
        Distribution<StateID>& result);

    void generate_all_transitions(
        const StateID& state,
        std::vector<OperatorID>& aops,
        std::vector<Distribution<StateID>>& successors);

    void print_statistics(std::ostream& out) const;

protected:
    void compute_successor_states(
        const State& s,
        OperatorID op_id,
        std::vector<WeightedElement<StateID>>& successors);

    void
    compute_applicable_operators(const State& s, std::vector<OperatorID>& ops);

    bool setup_cache(const StateID& state_id, CacheEntry& entry);

    CacheEntry& lookup(const StateID& state_id);
    CacheEntry& lookup(const StateID& state_id, bool& initialized);
};

} // namespace engine_interfaces

} // namespace probfd

#endif // __TRANSITION_GENERATOR_H__