#ifndef PROBFD_HEURISTICS_PDBS_TYPES_H
#define PROBFD_HEURISTICS_PDBS_TYPES_H

#include "pdbs/types.h"

namespace pdbs {
class PatternCollectionGenerator;
class PatternCollectionInformation;
} // namespace pdbs

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProjectionStateSpace;
class ProbabilityAwarePatternDatabase;

using PatternCollection = ::pdbs::PatternCollection;
using Pattern = ::pdbs::Pattern;
using PatternID = ::pdbs::PatternID;
using PatternSubCollection = std::vector<PatternID>;

using ProjectionCollection = std::vector<std::unique_ptr<ProjectionStateSpace>>;
using PPDBCollection =
    std::vector<std::shared_ptr<ProbabilityAwarePatternDatabase>>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __TYPES_H__