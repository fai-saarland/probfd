#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_PUCS_FLAW_FINDER_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_PUCS_FLAW_FINDER_H

#include "flaw_finding_strategy.h"

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
class PUCSFlawFinder : public FlawFindingStrategy<PDBType> {
public:
    ~PUCSFlawFinder() override = default;

    virtual FlawList apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        const ExplicitGState& init) override;

    std::string get_name() const override {
        return "PUCS Flaw Finder";
    }
};

}
}
}

#endif