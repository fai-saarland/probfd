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
class StateID;
class State;

namespace plugins {
class Options;
} // namespace plugins

namespace utils {
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

    std::vector<State> stk_;
    storage::PerStateStorage<ExplorationInfo> einfos_;

    const std::shared_ptr<utils::RandomNumberGenerator> rng_;
    const int max_search_states_;

public:
    explicit SamplingFlawFinder(const plugins::Options& opts);
    explicit SamplingFlawFinder(
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        int max_search_states);

    ~SamplingFlawFinder() override = default;

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

} // namespace probfd::pdbs::cegar

#endif // PROBFD_PDBS_CEGAR_SAMPLING_FLAW_FINDER_H
