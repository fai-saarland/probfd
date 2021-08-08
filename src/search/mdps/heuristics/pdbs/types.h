#pragma once

#include "../../../pdbs/types.h"

namespace pdbs {
class PatternCollectionGenerator;
class PatternCollectionInformation;
} // namespace pdbs

namespace probabilistic {
namespace pdbs {

namespace expected_cost {
class ExpCostProjection;
}

namespace maxprob {
class MaxProbProjection;
}

using PatternCollection = ::pdbs::PatternCollection;
using Pattern = ::pdbs::Pattern;
using PatternID = ::pdbs::PatternID;
using PatternClique = ::pdbs::PatternClique;

// TODO change this to an action 2d matrix type?
using VariableAdditivity = std::vector<std::vector<bool>>;

using ExpCostPDBCollection =
    std::vector<std::shared_ptr<expected_cost::ExpCostProjection>>;
using MaxProbPDBCollection =
    std::vector<std::shared_ptr<maxprob::MaxProbProjection>>;

} // namespace pdbs
} // namespace probabilistic
