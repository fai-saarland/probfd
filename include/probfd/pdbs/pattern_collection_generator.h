#ifndef PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_H
#define PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_H

#include "probfd/probabilistic_task.h"

// Forward Declarations
namespace probfd::pdbs {
class PatternCollectionInformation;
}

namespace probfd::pdbs {

class PatternCollectionGenerator {
public:
    virtual ~PatternCollectionGenerator() = default;

    virtual PatternCollectionInformation
    generate(const SharedProbabilisticTask& task) = 0;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_H
