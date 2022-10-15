#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_FLAW_FINDER_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_FLAW_FINDER_H

#include "probfd/heuristics/pdbs/pattern_selection/cegar/types.h"

#include <string>

namespace probfd {
namespace heuristics {
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

    // Second value returns whether policy is executable (modulo blacklisting)
    // Note that the flaw list might be empty regardless since only remaining
    // goals are added to the list for goal violations.
    virtual std::pair<FlawList, bool> apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        const ExplicitGState& init) = 0;

    virtual std::string get_name() const = 0;
};

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif