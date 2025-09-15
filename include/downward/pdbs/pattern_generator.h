#ifndef PDBS_PATTERN_GENERATOR_H
#define PDBS_PATTERN_GENERATOR_H

#include "downward/pdbs/pattern_collection_information.h"
#include "downward/pdbs/pattern_information.h"
#include "downward/pdbs/types.h"

#include "downward/utils/logging.h"

#include "downward/abstract_task.h"

#include <memory>
#include <string>

namespace downward::utils {
class RandomNumberGenerator;
}

namespace downward::pdbs {
class PatternCollectionGenerator {
    virtual std::string name() const = 0;
    virtual PatternCollectionInformation
    compute_patterns(const SharedAbstractTask& task) = 0;

protected:
    mutable utils::LogProxy log;

public:
    explicit PatternCollectionGenerator(utils::Verbosity verbosity);
    virtual ~PatternCollectionGenerator() = default;

    PatternCollectionInformation
    generate(const SharedAbstractTask& task);
};

class PatternGenerator {
    virtual std::string name() const = 0;
    virtual PatternInformation
    compute_pattern(const SharedAbstractTask& task) = 0;

protected:
    mutable utils::LogProxy log;

public:
    explicit PatternGenerator(utils::Verbosity verbosity);
    virtual ~PatternGenerator() = default;

    PatternInformation generate(const SharedAbstractTask& task);
};

} // namespace pdbs

#endif
