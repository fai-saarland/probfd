#ifndef MDPS_HEURISTICS_PDBS_TYPES_H
#define MDPS_HEURISTICS_PDBS_TYPES_H

#include "../../../pdbs/types.h"

namespace pdbs {
class PatternCollectionGenerator;
class PatternCollectionInformation;
} // namespace pdbs

namespace probfd {
namespace heuristics {

namespace pdbs {

class ExpCostProjection;
class MaxProbProjection;

using PatternCollection = ::pdbs::PatternCollection;
using Pattern = ::pdbs::Pattern;
using PatternID = ::pdbs::PatternID;
using PatternSubCollection = ::pdbs::PatternClique;

using VariableOrthogonality = std::vector<std::vector<bool>>;

template <class PDBType>
using PPDBCollection = std::vector<std::shared_ptr<PDBType>>;

using ExpCostPDBCollection = PPDBCollection<ExpCostProjection>;
using MaxProbPDBCollection = PPDBCollection<MaxProbProjection>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __TYPES_H__