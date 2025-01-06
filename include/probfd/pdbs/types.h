#ifndef PROBFD_PDBS_TYPES_H
#define PROBFD_PDBS_TYPES_H

#include "downward/pdbs/types.h"

namespace pdbs {
class PatternCollectionGenerator;
class PatternCollectionInformation;
} // namespace pdbs

namespace probfd {
template <typename, typename>
class MultiPolicy;
}

namespace probfd::pdbs {

class ProjectionStateSpace;
class ProbabilityAwarePatternDatabase;

using PatternCollection = ::pdbs::PatternCollection;
using Pattern = ::pdbs::Pattern;
using PatternID = ::pdbs::PatternID;
using PatternSubCollection = std::vector<PatternID>;

using ProjectionCollection = std::vector<std::unique_ptr<ProjectionStateSpace>>;
using PPDBCollection =
    std::vector<std::shared_ptr<ProbabilityAwarePatternDatabase>>;

using StateRank = int;
class ProjectionOperator;

using ProjectionMultiPolicy = MultiPolicy<StateRank, const ProjectionOperator*>;

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_TYPES_H
