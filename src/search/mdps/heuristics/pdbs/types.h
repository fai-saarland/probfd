#pragma once

#include "../../../pdbs/types.h"

namespace pdbs {
class PatternCollectionGenerator;
class PatternCollectionInformation;
} // namespace pdbs

namespace probabilistic {
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
} // namespace probabilistic
