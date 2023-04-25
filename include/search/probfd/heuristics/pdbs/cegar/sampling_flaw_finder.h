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

    std::vector<State> stk;
    storage::PerStateStorage<ExplorationInfo> einfos;

    const int max_search_states;

    static constexpr unsigned STATE_PUSHED = 1;
    static constexpr unsigned FLAW_OCCURED = 1 << 1;

public:
    explicit SamplingFlawFinder(options::Options& opts);
    explicit SamplingFlawFinder(int max_search_states);

    ~SamplingFlawFinder() override = default;

    bool apply_policy(
        CEGAR& base,
        const ProbabilisticTaskProxy& task_proxy,
        int solution_index,
        std::vector<Flaw>& flaw_list,
        utils::CountdownTimer& timer) override;

    std::string get_name() override;

private:
    unsigned int push_state(
        CEGAR& base,
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