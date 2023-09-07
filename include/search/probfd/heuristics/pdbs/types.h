#ifndef PROBFD_HEURISTICS_PDBS_TYPES_H
#define PROBFD_HEURISTICS_PDBS_TYPES_H

#include "probfd/heuristics/abstractions/types.h"

#include "downward/pdbs/types.h"

namespace pdbs {
class PatternCollectionGenerator;
class PatternCollectionInformation;
} // namespace pdbs

namespace probfd {
namespace heuristics {
namespace pdbs {

using PatternCollection = ::pdbs::PatternCollection;
using Pattern = ::pdbs::Pattern;
using PatternID = ::pdbs::PatternID;
using PatternSubCollection = std::vector<PatternID>;

class ProbabilityAwarePatternDatabase;
class ProjectionStateSpace;
class ProjectionOperator;

using PPDBCollection =
    std::vector<std::shared_ptr<ProbabilityAwarePatternDatabase>>;

using abstractions::AbstractionEvaluator;
using abstractions::AbstractStateIndex;

// using ProjectionCollection =
//     abstractions::AbstractionCollection<const ProjectionOperator*>;
using ProjectionCollection = std::vector<std::unique_ptr<ProjectionStateSpace>>;

using ProjectionMultiPolicy =
    abstractions::AbstractMultiPolicy<const ProjectionOperator*>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __TYPES_H__