#ifndef PROBFD_PDBS_CEGAR_SAMPLING_FLAW_FINDER_H
#define PROBFD_PDBS_CEGAR_SAMPLING_FLAW_FINDER_H

#include "probfd/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/distribution.h"

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

// Forward Declarations
namespace downward {
class StateID;
class State;
}

namespace downward::utils {
class RandomNumberGenerator;
}

namespace probfd {
class ProbabilisticTaskProxy;
}

namespace probfd::pdbs::cegar {

class SamplingFlawFinder : public FlawFindingStrategy {
    struct ExplorationInfo {
        bool explored = false;
        Distribution<StateID> successors;
    };

    std::vector<downward::State> stk_;
    storage::PerStateStorage<ExplorationInfo> einfos_;

    const std::shared_ptr<downward::utils::RandomNumberGenerator> rng_;
    const int max_search_states_;

public:
    explicit SamplingFlawFinder(
        std::shared_ptr<downward::utils::RandomNumberGenerator> rng,
        int max_search_states);

    ~SamplingFlawFinder() override;

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

#endif // PROBFD_PDBS_CEGAR_SAMPLING_FLAW_FINDER_H
