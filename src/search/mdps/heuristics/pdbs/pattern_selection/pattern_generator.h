#pragma once

#include "../../../../operator_cost.h"
#include "../../../../utils/printable.h"
#include "../types.h"

#include "pattern_collection_information.h"

#include <memory>

namespace probabilistic {
namespace pdbs {

/// Namespace dedicated to pattern selection techniques for probabilistic PDBs
namespace pattern_selection {

template <class PDBType>
class PatternCollectionGenerator {
public:
    virtual ~PatternCollectionGenerator() = default;

    virtual PatternCollectionInformation<PDBType>
    generate(OperatorCost cost_type) = 0;

    virtual std::shared_ptr<utils::Printable> get_report() const;
};

using ExpCostPatternCollectionGenerator =
    PatternCollectionGenerator<ExpCostProjection>;

using MaxProbPatternCollectionGenerator =
    PatternCollectionGenerator<MaxProbProjection>;

} // namespace pattern_selection
} // namespace pdbs
} // namespace probabilistic