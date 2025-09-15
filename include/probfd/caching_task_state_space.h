#ifndef PROBFD_CACHING_TASK_STATE_SPACE_H
#define PROBFD_CACHING_TASK_STATE_SPACE_H

#include "probfd/task_state_space.h"

#include "probfd/storage/segmented_memory_pool.h"

#include "probfd/fdr_types.h"
#include "probfd/state_id.h"

#include "downward/per_state_information.h"

#include <limits>
#include <memory>
#include <vector>

// Forward Declarations
namespace downward {
class Evaluator;
class State;
class OperatorID;
} // namespace downward

namespace utils {
class LogProxy;
}

namespace probfd {
struct SuccessorDistribution;
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
        downward::OperatorID* aops = nullptr;
        StateID* succs = nullptr;
    };

    downward::PerStateInformation<CacheEntry> cache_;
    storage::SegmentedMemoryPool<> cache_data_;

    std::vector<downward::OperatorID> aops_;
    std::vector<StateID> successors_;

public:
    explicit CachingTaskStateSpace(
        const downward::VariableSpace& variables,
        const downward::AxiomSpace& axioms,
        std::shared_ptr<ProbabilisticOperatorSpace> operators,
        const downward::InitialStateValues& initial_values,
        std::vector<std::shared_ptr<downward::Evaluator>>
            path_dependent_evaluators);

    void generate_applicable_actions(
        const downward::State& state,
        std::vector<downward::OperatorID>& result) final;

    void generate_action_transitions(
        const downward::State& state,
        downward::OperatorID operator_id,
        SuccessorDistribution& successor_dist) final;

    void generate_all_transitions(
        const downward::State& state,
        std::vector<downward::OperatorID>& aops,
        std::vector<SuccessorDistribution>& successor_dist) final;

    void generate_all_transitions(
        const downward::State& state,
        std::vector<TransitionTailType>& transitions) final;

    void print_statistics(std::ostream& out) const final;

private:
    void compute_successor_states(
        const downward::State& s,
        downward::OperatorID op_id,
        std::vector<StateID>& successors);

    void setup_cache(const downward::State& state, CacheEntry& entry);

    CacheEntry& lookup(const downward::State& state);
};

} // namespace probfd

#endif // PROBFD_CACHING_TASK_STATE_SPACE_H