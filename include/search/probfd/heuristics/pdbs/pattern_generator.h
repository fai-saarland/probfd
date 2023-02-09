#ifndef PROBFD_HEURISTICS_PDBS_PATTERN_GENERATOR_H
#define PROBFD_HEURISTICS_PDBS_PATTERN_GENERATOR_H

#include "probfd/heuristics/pdbs/pattern_collection_information.h"

#include "probfd/heuristics/pdbs/types.h"

#include "utils/printable.h"

#include "operator_cost.h"

#include <memory>

namespace probfd {
class ProbabilisticTask;
namespace heuristics {
namespace pdbs {

template <class PDBType>
class PatternCollectionGenerator {
public:
    virtual ~PatternCollectionGenerator() = default;

    virtual PatternCollectionInformation<PDBType>
    generate(const std::shared_ptr<ProbabilisticTask>& task) = 0;

    virtual std::shared_ptr<utils::Printable> get_report() const;
};

using ExpCostPatternCollectionGenerator =
    PatternCollectionGenerator<SSPPatternDatabase>;

using MaxProbPatternCollectionGenerator =
    PatternCollectionGenerator<MaxProbPatternDatabase>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
#endif // __PATTERN_GENERATOR_H__