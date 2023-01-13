#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_BFS_FLAW_FINDER_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_BFS_FLAW_FINDER_H

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include <stack>
#include <unordered_set>

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

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

    virtual bool apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        std::vector<int>& state,
        std::vector<Flaw>& flaw_list) override;

private:
    bool expand(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        std::vector<int>& state,
        std::vector<Flaw>& flaw_list);
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif