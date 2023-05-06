#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_BFS_FLAW_FINDER_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_BFS_FLAW_FINDER_H

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/storage/per_state_storage.h"

#include <stack>
#include <unordered_set>

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

class BFSFlawFinder : public FlawFindingStrategy {
    std::deque<State> open;
    storage::PerStateStorage<bool> closed;

    const int max_search_states;

public:
    explicit BFSFlawFinder(options::Options& opts);
    explicit BFSFlawFinder(int max_search_states);

    ~BFSFlawFinder() override = default;

    bool apply_policy(
        const CEGAR& base,
        const ProbabilisticTaskProxy& task_proxy,
        int solution_index,
        std::vector<Flaw>& flaw_list,
        utils::CountdownTimer& timer) override;

    std::string get_name() override;
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif