#ifndef PDBS_PATTERN_GENERATOR_H
#define PDBS_PATTERN_GENERATOR_H

#include "pdbs/pattern_collection_information.h"
#include "pdbs/pattern_information.h"
#include "pdbs/types.h"

#include "utils/logging.h"

#include <memory>
#include <string>

class AbstractTask;

namespace plugins {
class Feature;
class Options;
} // namespace plugins

namespace utils {
class RandomNumberGenerator;
}

namespace pdbs {
class PatternCollectionGenerator {
    virtual std::string name() const = 0;
    virtual PatternCollectionInformation compute_patterns(
        const std::shared_ptr<AbstractTask> &task) = 0;
protected:
    mutable utils::LogProxy log;
public:
    explicit PatternCollectionGenerator(const plugins::Options& opts);
    virtual ~PatternCollectionGenerator() = default;

    PatternCollectionInformation generate(
        const std::shared_ptr<AbstractTask> &task);
};

class PatternGenerator {
    virtual std::string name() const = 0;
    virtual PatternInformation compute_pattern(
        const std::shared_ptr<AbstractTask> &task) = 0;
protected:
    mutable utils::LogProxy log;
public:
    explicit PatternGenerator(const plugins::Options& opts);
    virtual ~PatternGenerator() = default;

    PatternInformation generate(const std::shared_ptr<AbstractTask> &task);
};

extern void add_generator_options_to_feature(plugins::Feature& feature);
}

#endif
