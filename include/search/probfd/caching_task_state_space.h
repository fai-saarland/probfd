#ifndef PROBFD_CACHING_TASK_STATE_SPACE_H
#define PROBFD_CACHING_TASK_STATE_SPACE_H

#include "probfd/task_state_space.h"

namespace probfd {

class CachingTaskStateSpace : public InducedTaskStateSpace {
    struct CacheEntry {
        bool is_initialized() const
        {
            return naops != std::numeric_limits<unsigned>::max();
        }

        unsigned naops = std::numeric_limits<unsigned>::max();
        OperatorID* aops = nullptr;
        StateID* succs = nullptr;
    };

    storage::PerStateStorage<CacheEntry> cache_;
    storage::SegmentedMemoryPool<> cache_data_;

public:
    CachingTaskStateSpace(
        std::shared_ptr<ProbabilisticTask> task,
        utils::LogProxy log,
        TaskSimpleCostFunction* cost_function,
        const std::vector<std::shared_ptr<::Evaluator>>&
            path_dependent_evaluators);

    void generate_applicable_actions(
        StateID state_id,
        std::vector<OperatorID>& result) override final;

    void generate_action_transitions(
        StateID state,
        OperatorID operator_id,
        Distribution<StateID>& result) override final;

    void generate_all_transitions(
        StateID state,
        std::vector<OperatorID>& aops,
        std::vector<Distribution<StateID>>& successors) override final;

    void generate_all_transitions(
        StateID state,
        std::vector<Transition>& transitions) override final;

    void print_statistics() const override final;

private:
    void compute_successor_states(
        const State& s,
        OperatorID op_id,
        std::vector<StateID>& successors);

    bool setup_cache(StateID state_id, CacheEntry& entry);

    CacheEntry& lookup(StateID state_id);
    CacheEntry& lookup(StateID state_id, bool& initialized);
};

} // namespace probfd

#endif // __TRANSITION_GENERATOR_H__