#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_PATTERN_GENERATOR_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_PATTERN_GENERATOR_H

#include "probfd/heuristics/pdbs/pattern_selection/pattern_collection_information.h"

#include "probfd/heuristics/pdbs/types.h"

#include "utils/printable.h"

#include "operator_cost.h"

#include <memory>

namespace probfd {
class ProbabilisticTask;
namespace heuristics {
namespace pdbs {

/// Namespace dedicated to pattern selection techniques for probabilistic PDBs
namespace pattern_selection {

template <class PDBType>
class PatternCollectionGenerator {
public:
    virtual ~PatternCollectionGenerator() = default;

    virtual PatternCollectionInformation<PDBType>
    generate(const std::shared_ptr<ProbabilisticTask>& task) = 0;

    virtual std::shared_ptr<utils::Printable> get_report() const;
};

using ExpCostPatternCollectionGenerator =
    PatternCollectionGenerator<ExpCostProjection>;

using MaxProbPatternCollectionGenerator =
    PatternCollectionGenerator<MaxProbProjection>;

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd
#endif // __PATTERN_GENERATOR_H__