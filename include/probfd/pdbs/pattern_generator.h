#ifndef PROBFD_PDBS_PATTERN_GENERATOR_H
#define PROBFD_PDBS_PATTERN_GENERATOR_H

#include "probfd/probabilistic_task.h"

// Forward Declarations
namespace probfd::pdbs {
class PatternInformation;
}

namespace probfd::pdbs {

class PatternGenerator {
    virtual ~PatternGenerator() = default;

    virtual PatternInformation
    generate(const SharedProbabilisticTask& task) = 0;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PATTERN_GENERATOR_H
