#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_BFS_FLAW_FINDER_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_BFS_FLAW_FINDER_H

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

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

class BFSFlawFinder : public FlawFindingStrategy {
    std::deque<State> open;
    storage::PerStateStorage<bool> closed;

    const int max_search_states;

public:
    explicit BFSFlawFinder(const plugins::Options& opts);
    explicit BFSFlawFinder(int max_search_states);

    ~BFSFlawFinder() override = default;

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

#endif