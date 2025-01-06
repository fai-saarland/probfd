#ifndef PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_H
#define PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_H

#include "probfd/fdr_types.h"

#include "downward/utils/logging.h"

#include <memory>

// Forward Declarations
namespace probfd {
class ProbabilisticTask;
}

namespace probfd::pdbs {
class PatternCollectionInformation;
}

namespace probfd::pdbs {

class PatternCollectionGenerator {
protected:
    mutable utils::LogProxy log_;

public:
    explicit PatternCollectionGenerator(utils::Verbosity verbosity);
    explicit PatternCollectionGenerator(utils::LogProxy log);
    virtual ~PatternCollectionGenerator() = default;

    virtual PatternCollectionInformation generate(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) = 0;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_H
