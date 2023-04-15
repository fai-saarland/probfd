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

    const unsigned violation_threshold;

public:
    BFSFlawFinder(options::Options& opts);
    BFSFlawFinder(unsigned violation_threshold);

    ~BFSFlawFinder() override = default;

    virtual bool apply_policy(
        PatternCollectionGeneratorCegar& base,
        const ProbabilisticTaskProxy& task_proxy,
        int solution_index,
        std::vector<Flaw>& flaw_list,
        utils::CountdownTimer& timer) override;

    virtual std::string get_name() override;

private:
    bool expand(
        PatternCollectionGeneratorCegar& base,
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