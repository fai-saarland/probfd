#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_MLP_EXPLORATION_STRATEGY_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_MLP_EXPLORATION_STRATEGY_H

#include "probfd/heuristics/pdbs/cegar/policy_exploration_strategy.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/value_type.h"

#include "downward/algorithms/priority_queues.h"

#include "downward/task_proxy.h"

class State;
class StateRegistry;

namespace plugins {
class Options;
} // namespace plugins

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

class MLPExplorationStrategy : public PolicyExplorationStrategy {
    struct ExpansionInfo {
        bool expanded = false;
        value_t path_probability = 0_vt;
    };

    priority_queues::HeapQueue<value_t, State> pq;
    storage::PerStateStorage<ExpansionInfo> probabilities;

    const int max_search_states;

public:
    explicit MLPExplorationStrategy(const plugins::Options& opts);
    explicit MLPExplorationStrategy(int max_search_states);

    ~MLPExplorationStrategy() override = default;

    bool apply_policy(
        ProbabilisticTaskProxy task_proxy,
        const ProjectionStateSpace& mdp,
        const ProbabilityAwarePatternDatabase& pdb,
        const ProjectionMultiPolicy& policy,
        FlawFilter& flaw_filter,
        std::vector<Flaw>& flaw_list,
        utils::CountdownTimer& timer) override;

    std::string get_name() override;
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_CEGAR_MLP_EXPLORATION_STRATEGY_H
