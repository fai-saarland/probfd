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

class SSPPatternDatabase;
class MaxProbPatternDatabase;

using PatternCollection = ::pdbs::PatternCollection;
using Pattern = ::pdbs::Pattern;
using PatternID = ::pdbs::PatternID;
using PatternSubCollection = ::pdbs::PatternClique;

template <class PDBType>
using PPDBCollection = std::vector<std::shared_ptr<PDBType>>;

using ExpCostPDBCollection = PPDBCollection<SSPPatternDatabase>;
using MaxProbPDBCollection = PPDBCollection<MaxProbPatternDatabase>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __TYPES_H__