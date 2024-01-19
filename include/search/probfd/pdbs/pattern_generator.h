#ifndef PROBFD_PDBS_PATTERN_GENERATOR_H
#define PROBFD_PDBS_PATTERN_GENERATOR_H

#include "downward/utils/logging.h"

#include <memory>

// Forward Declarations
namespace plugins {
class Feature;
class Options;
} // namespace plugins

namespace probfd {
class ProbabilisticTask;
}

namespace probfd::pdbs {
class PatternInformation;
}

namespace probfd::pdbs {

class PatternGenerator {
protected:
    mutable utils::LogProxy log_;

public:
    explicit PatternGenerator(const plugins::Options& opts);
    explicit PatternGenerator(utils::LogProxy log);

    virtual ~PatternGenerator() = default;

    virtual PatternInformation
    generate(const std::shared_ptr<ProbabilisticTask>& task) = 0;
};

extern void add_pattern_generator_options_to_feature(plugins::Feature& feature);

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PATTERN_GENERATOR_H
