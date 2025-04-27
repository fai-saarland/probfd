#ifndef PROBFD_PDBS_PATTERN_GENERATOR_H
#define PROBFD_PDBS_PATTERN_GENERATOR_H

#include "downward/utils/logging.h"
#include "probfd/probabilistic_task.h"

#include <memory>

// Forward Declarations
namespace probfd::pdbs {
class PatternInformation;
}

namespace probfd::pdbs {

class PatternGenerator {
protected:
    mutable downward::utils::LogProxy log_;

public:
    explicit PatternGenerator(downward::utils::Verbosity);
    explicit PatternGenerator(downward::utils::LogProxy log);

    virtual ~PatternGenerator() = default;

    virtual PatternInformation
    generate(const SharedProbabilisticTask& task) = 0;
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PATTERN_GENERATOR_H
