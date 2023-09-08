#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_SAMPLING_FLAW_FINDER_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_SAMPLING_FLAW_FINDER_H

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/distribution.h"

#include <vector>

class StateID;
class State;
class StateRegistry;

namespace plugins {
class Options;
} // namespace plugins

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

class SamplingFlawFinder : public FlawFindingStrategy {
    struct ExplorationInfo {
        bool explored = false;
        Distribution<StateID> successors;
    };

    std::vector<State> stk;
    storage::PerStateStorage<ExplorationInfo> einfos;

    const std::shared_ptr<utils::RandomNumberGenerator> rng;
    const int max_search_states;

public:
    explicit SamplingFlawFinder(const plugins::Options& opts);
    explicit SamplingFlawFinder(
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        int max_search_states);

    ~SamplingFlawFinder() override = default;

    bool apply_policy(
        ProbabilisticTaskProxy task_proxy,
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
} // namespace heuristics
} // namespace probfd

#endif