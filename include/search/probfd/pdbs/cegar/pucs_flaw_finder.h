#ifndef PROBFD_PDBS_CEGAR_PUCS_FLAW_FINDER_H
#define PROBFD_PDBS_CEGAR_PUCS_FLAW_FINDER_H

#include "probfd/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/pdbs/types.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/value_type.h"

#include "downward/algorithms/priority_queues.h"

#include "downward/task_proxy.h"

#include <string>
#include <unordered_set>
#include <vector>

// Forward Declarations
class State;

namespace probfd::pdbs::cegar {

class PUCSFlawFinder : public FlawFindingStrategy {
    struct ExpansionInfo {
        bool expanded = false;
        value_t path_probability = 0_vt;
    };

    priority_queues::HeapQueue<value_t, State> pq_;
    storage::PerStateStorage<ExpansionInfo> probabilities_;

    const int max_search_states_;

public:
    explicit PUCSFlawFinder(int max_search_states);

    bool apply_policy(
        const ProbabilisticTaskProxy& task_proxy,
        const StateRankingFunction& state_ranking_function,
        const ProjectionStateSpace& mdp,
        const ProjectionMultiPolicy& policy,
        std::vector<Flaw>& flaws,
        const std::function<bool(const Flaw&)>& notify_flaw,
        utils::CountdownTimer& timer) override;

    std::string get_name() override;
};

} // namespace probfd::pdbs::cegar

#endif // PROBFD_PDBS_CEGAR_PUCS_FLAW_FINDER_H
