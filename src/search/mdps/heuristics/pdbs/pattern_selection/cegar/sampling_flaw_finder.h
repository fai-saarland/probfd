#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_SAMPLING_FLAW_FINDER_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_SAMPLING_FLAW_FINDER_H

#include "flaw_finding_strategy.h"

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
class SamplingFlawFinder : public FlawFindingStrategy<PDBType> {
public:
    ~SamplingFlawFinder() override = default;

    virtual FlawList apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        const ExplicitGState& init) override;

    std::string get_name() const override {
        return "Sampling Flaw Finder";
    }
};

}
}
}

#endif