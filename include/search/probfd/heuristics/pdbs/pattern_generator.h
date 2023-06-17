#ifndef PROBFD_HEURISTICS_PDBS_PATTERN_GENERATOR_H
#define PROBFD_HEURISTICS_PDBS_PATTERN_GENERATOR_H

#include "probfd/heuristics/pdbs/pattern_collection_information.h"
#include "probfd/heuristics/pdbs/pattern_information.h"

#include "probfd/heuristics/pdbs/types.h"

#include "downward/utils/logging.h"
#include "downward/utils/printable.h"

#include "downward/operator_cost.h"

#include <memory>

namespace probfd {
class ProbabilisticTask;
namespace heuristics {
namespace pdbs {

class PatternCollectionGenerator {
protected:
    mutable utils::LogProxy log;

public:
    explicit PatternCollectionGenerator(const plugins::Options& opts);
    explicit PatternCollectionGenerator(const utils::LogProxy& log);
    virtual ~PatternCollectionGenerator() = default;

    virtual PatternCollectionInformation
    generate(const std::shared_ptr<ProbabilisticTask>& task) = 0;
};

class PatternGenerator {
protected:
    mutable utils::LogProxy log;

public:
    explicit PatternGenerator(const plugins::Options& opts);
    explicit PatternGenerator(const utils::LogProxy& log);
    virtual ~PatternGenerator() = default;

    virtual PatternInformation
    generate(const std::shared_ptr<ProbabilisticTask>& task) = 0;
};

extern void add_generator_options_to_feature(plugins::Feature& feature);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
#endif // __PATTERN_GENERATOR_H__