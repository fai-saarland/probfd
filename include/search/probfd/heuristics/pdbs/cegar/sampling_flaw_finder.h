#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_SAMPLING_FLAW_FINDER_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_SAMPLING_FLAW_FINDER_H

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/storage/per_state_storage.h"

#include "probfd/distribution.h"

#include <stack>
#include <unordered_map>

class StateID;
class State;
class StateRegistry;

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

class SamplingFlawFinder : public FlawFindingStrategy {
    struct ExplorationInfo {
        bool explored = false;
        Distribution<StateID> successors;
    };

    std::stack<State> stk;
    storage::PerStateStorage<ExplorationInfo> einfos;

    unsigned violation_threshold;

    static constexpr unsigned STATE_PUSHED = 1;
    static constexpr unsigned FLAW_OCCURED = 1 << 1;

public:
    explicit SamplingFlawFinder(options::Options& opts);
    explicit SamplingFlawFinder(unsigned violation_threshold);
    ~SamplingFlawFinder() override = default;

    virtual bool apply_policy(
        PatternCollectionGeneratorDisjointCegar& base,
        const ProbabilisticTaskProxy& task_proxy,
        int solution_index,
        std::vector<Flaw>& flaw_list,
        utils::CountdownTimer& timer) override;

    virtual std::string get_name() override;

private:
    unsigned int push_state(
        PatternCollectionGeneratorDisjointCegar& base,
        const ProbabilisticTaskProxy& task_proxy,
        int solution_index,
        State state,
        std::vector<Flaw>& flaw_list,
        StateRegistry& registry);
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif