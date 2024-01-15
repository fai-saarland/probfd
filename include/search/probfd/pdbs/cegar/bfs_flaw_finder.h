#ifndef PROBFD_PDBS_CEGAR_BFS_FLAW_FINDER_H
#define PROBFD_PDBS_CEGAR_BFS_FLAW_FINDER_H

#include "probfd/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/pdbs/types.h"

#include "probfd/storage/per_state_storage.h"

#include "downward/task_proxy.h"

#include <deque>
#include <string>
#include <unordered_set>
#include <vector>

namespace plugins {
class Options;
} // namespace plugins

namespace probfd {
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
        const ProbabilisticTaskProxy& task_proxy,
        const ProjectionStateSpace& mdp,
        const ProbabilityAwarePatternDatabase& pdb,
        const ProjectionMultiPolicy& policy,
        const std::unordered_set<int>& blacklisted_variables,
        std::vector<Flaw>& flaw_list,
        utils::CountdownTimer& timer) override;

    std::string get_name() override;
};

} // namespace cegar
} // namespace pdbs
} // namespace probfd

#endif // PROBFD_PDBS_CEGAR_BFS_FLAW_FINDER_H
