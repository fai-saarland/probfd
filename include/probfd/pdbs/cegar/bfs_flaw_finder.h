#ifndef PROBFD_PDBS_CEGAR_BFS_FLAW_FINDER_H
#define PROBFD_PDBS_CEGAR_BFS_FLAW_FINDER_H

#include "probfd/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/pdbs/types.h"

#include "probfd/storage/per_state_storage.h"

#include "downward/state.h"

#include <deque>
#include <string>
#include <unordered_set>
#include <vector>

namespace probfd::pdbs::cegar {

class BFSFlawFinder : public FlawFindingStrategy {
    std::deque<downward::State> open_;
    storage::PerStateStorage<bool> closed_;

    const int max_search_states_;

public:
    explicit BFSFlawFinder(int max_search_states);

    bool apply_policy(
        const ProbabilisticTask& task,
        const StateRankingFunction& state_ranking_function,
        const ProjectionStateSpace& mdp,
        const ProjectionMultiPolicy& policy,
        std::vector<Flaw>& flaws,
        const std::function<bool(const Flaw&)>& notify_flaw,
        downward::utils::CountdownTimer& timer) override;

    std::string get_name() override;
};

} // namespace probfd::pdbs::cegar

#endif // PROBFD_PDBS_CEGAR_BFS_FLAW_FINDER_H
