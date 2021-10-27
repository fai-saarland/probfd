#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_FLAW_FINDER_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_FLAW_FINDER_H

#include "types.h"

#include <string>

namespace probabilistic {
namespace pdbs {

class AbstractPolicy;
class ProbabilisticProjection;

namespace pattern_selection {

template <typename PDBType>
class PatternCollectionGeneratorCegar;

template <typename PDBType>
class FlawFindingStrategy {
public:
    virtual ~FlawFindingStrategy() = default;
    
    // Second return value specified if there was a goal violation
    virtual std::pair<FlawList, bool> apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        const ExplicitGState& init) = 0;

    virtual std::string get_name() const = 0;
};

}
}
}

#endif