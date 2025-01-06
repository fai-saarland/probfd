#ifndef PROBFD_CACHING_TASK_STATE_SPACE_H
#define PROBFD_CACHING_TASK_STATE_SPACE_H

#include "probfd/task_state_space.h"

#include "probfd/storage/segmented_memory_pool.h"

#include "probfd/fdr_types.h"
#include "probfd/types.h"

#include "downward/per_state_information.h"

#include <limits>
#include <memory>
#include <vector>

// Forward Declarations
class Evaluator;
class State;
class OperatorID;

namespace utils {
class LogProxy;
}

namespace probfd {
class ProbabilisticTask;
template <typename>
class Distribution;
} // namespace probfd

namespace probfd {

class CachingTaskStateSpace : public TaskStateSpace {
    struct CacheEntry {
        [[nodiscard]]
        bool is_initialized() const
        {
            return naops != std::numeric_limits<unsigned>::max();
        }

        unsigned naops = std::numeric_limits<unsigned>::max();
        OperatorID* aops = nullptr;
        StateID* succs = nullptr;
    };

    PerStateInformation<CacheEntry> cache_;
    storage::SegmentedMemoryPool<> cache_data_;

    std::vector<OperatorID> aops_;
    std::vector<StateID> successors_;

public:
    CachingTaskStateSpace(
        std::shared_ptr<ProbabilisticTask> task,
        utils::LogProxy log,
        std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators);

    void generate_applicable_actions(
        const State& state,
        std::vector<OperatorID>& result) final;

    void generate_action_transitions(
        const State& state,
        OperatorID operator_id,
        Distribution<StateID>& result) final;

    void generate_all_transitions(
        const State& state,
        std::vector<OperatorID>& aops,
        std::vector<Distribution<StateID>>& successors) final;

    void generate_all_transitions(
        const State& state,
        std::vector<TransitionType>& transitions) final;

    void print_statistics() const final;

private:
    void compute_successor_states(
        const State& s,
        OperatorID op_id,
        std::vector<StateID>& successors);

    void setup_cache(const State& state, CacheEntry& entry);

    CacheEntry& lookup(const State& state);
};

} // namespace probfd

#endif // PROBFD_CACHING_TASK_STATE_SPACE_H