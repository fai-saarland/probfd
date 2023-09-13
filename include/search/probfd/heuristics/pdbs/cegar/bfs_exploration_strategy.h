#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_BFS_EXPLORATION_STRATEGY_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_BFS_EXPLORATION_STRATEGY_H

#include "probfd/heuristics/pdbs/cegar/policy_exploration_strategy.h"

#include "probfd/storage/per_state_storage.h"

#include <deque>
#include <unordered_set>

class State;
class StateRegistry;

namespace plugins {
class Options;
} // namespace plugins

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

class BFSExplorationStrategy : public PolicyExplorationStrategy {
    std::deque<State> open;
    storage::PerStateStorage<bool> closed;

    const int max_search_states;

public:
    explicit BFSExplorationStrategy(const plugins::Options& opts);
    explicit BFSExplorationStrategy(int max_search_states);

    ~BFSExplorationStrategy() override = default;

    bool apply_policy(
        ProbabilisticTaskProxy task_proxy,
        const ProjectionInfo& projection,
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

#endif // PROBFD_HEURISTICS_PDBS_CEGAR_BFS_EXPLORATION_STRATEGY_H
