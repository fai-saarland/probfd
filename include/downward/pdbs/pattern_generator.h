#ifndef PDBS_PATTERN_GENERATOR_H
#define PDBS_PATTERN_GENERATOR_H

#include "downward/pdbs/pattern_collection_information.h"
#include "downward/pdbs/pattern_information.h"
#include "downward/pdbs/types.h"

#include "downward/utils/logging.h"

#include <memory>
#include <string>

namespace downward {
class AbstractTask;
}

namespace downward::utils {
class RandomNumberGenerator;
}

namespace downward::pdbs {
class PatternCollectionGenerator {
    virtual std::string name() const = 0;
    virtual PatternCollectionInformation
    compute_patterns(const std::shared_ptr<AbstractTask>& task) = 0;

protected:
    mutable utils::LogProxy log;

public:
    explicit PatternCollectionGenerator(utils::Verbosity verbosity);
    virtual ~PatternCollectionGenerator() = default;

    PatternCollectionInformation
    generate(const std::shared_ptr<AbstractTask>& task);
};

class PatternGenerator {
    virtual std::string name() const = 0;
    virtual PatternInformation
    compute_pattern(const std::shared_ptr<AbstractTask>& task) = 0;

protected:
    mutable utils::LogProxy log;

public:
    explicit PatternGenerator(utils::Verbosity verbosity);
    virtual ~PatternGenerator() = default;

    PatternInformation generate(const std::shared_ptr<AbstractTask>& task);
};

} // namespace pdbs

#endif
