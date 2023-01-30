#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_BFS_FLAW_FINDER_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_BFS_FLAW_FINDER_H

#include "probfd/heuristics/pdbs/pattern_selection/cegar/flaw_finding_strategy.h"

#include <stack>
#include <unordered_set>

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
class BFSFlawFinder : public FlawFindingStrategy<PDBType> {
    std::deque<std::vector<int>> open;
    std::unordered_set<
        std::vector<int>,
        typename FlawFindingStrategy<PDBType>::StateHash>
        closed;

    const unsigned int violation_threshold;

public:
    BFSFlawFinder(
        const ProbabilisticTask* task,
        unsigned int violation_threshold);

    ~BFSFlawFinder() override = default;

    virtual std::pair<FlawList, bool> apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        std::vector<int>& state) override;

private:
    bool expand(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        std::vector<int>& state,
        FlawList& flaw_list);
};

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif