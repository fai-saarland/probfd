#ifndef PROBFD_CACHING_TASK_STATE_SPACE_H
#define PROBFD_CACHING_TASK_STATE_SPACE_H

#include "probfd/task_state_space.h"

namespace probfd {

class CachingTaskStateSpace : public TaskStateSpace {
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
        const std::vector<std::shared_ptr<::Evaluator>>&
            path_dependent_evaluators);

    StateID get_state_id(const State& state) override;
    State get_state(StateID state_id) override;

    void generate_applicable_actions(
        StateID state_id,
        std::vector<OperatorID>& result) override;

    void generate_action_transitions(
        StateID state,
        OperatorID operator_id,
        Distribution<StateID>& result) override;

    void generate_all_transitions(
        StateID state,
        std::vector<OperatorID>& aops,
        std::vector<Distribution<StateID>>& successors) override;

    void print_statistics(std::ostream& out) const override;

protected:
    bool setup_cache(StateID state_id, CacheEntry& entry);

    CacheEntry& lookup(StateID state_id);
    CacheEntry& lookup(StateID state_id, bool& initialized);
};

} // namespace probfd

#endif // __TRANSITION_GENERATOR_H__