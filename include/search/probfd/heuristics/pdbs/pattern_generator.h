#ifndef PROBFD_HEURISTICS_PDBS_PATTERN_GENERATOR_H
#define PROBFD_HEURISTICS_PDBS_PATTERN_GENERATOR_H

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

#include <memory>

namespace probfd {
class ProbabilisticTask;
namespace heuristics {
namespace pdbs {

class PatternInformation;

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

extern void add_pattern_generator_options_to_feature(plugins::Feature& feature);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
#endif // __PATTERN_GENERATOR_H__