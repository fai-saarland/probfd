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

class ProbabilisticPatternDatabase;

using PatternCollection = ::pdbs::PatternCollection;
using Pattern = ::pdbs::Pattern;
using PatternID = ::pdbs::PatternID;
using PatternSubCollection = std::vector<PatternID>;

using PPDBCollection =
    std::vector<std::shared_ptr<ProbabilisticPatternDatabase>>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __TYPES_H__