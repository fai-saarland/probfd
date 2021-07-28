#pragma once

#include "../../../pdbs/types.h"

namespace pdbs {
class PatternCollectionGenerator;
class PatternCollectionInformation;
} // namespace pdbs

namespace probabilistic::pdbs {

class ExpCostProjection;
class MaxProbProjection;

using PatternCollection = ::pdbs::PatternCollection;
using Pattern = ::pdbs::Pattern;
using PatternID = ::pdbs::PatternID;
using PatternClique = ::pdbs::PatternClique;

// TODO change this to an action 2d matrix type?
using VariableAdditivity = std::vector<std::vector<bool>>;

using ExpCostPDBCollection = std::vector<std::shared_ptr<ExpCostProjection>>;
using MaxProbPDBCollection = std::vector<std::shared_ptr<MaxProbProjection>>;

} // namespace probabilistic::pdbs
