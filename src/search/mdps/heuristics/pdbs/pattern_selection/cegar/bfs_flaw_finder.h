#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_BFS_FLAW_FINDER_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_BFS_FLAW_FINDER_H

#include "flaw_finding_strategy.h"

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
class BFSFlawFinder : public FlawFindingStrategy<PDBType> {
public:
    ~BFSFlawFinder() override = default;

    virtual std::pair<FlawList, bool> apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        const ExplicitGState& init) override;

    std::string get_name() const override {
        return "BFS Flaw Finder";
    }
};

}
}
}

#endif