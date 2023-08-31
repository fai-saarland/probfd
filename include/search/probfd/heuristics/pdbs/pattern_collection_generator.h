#ifndef PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_GENERATOR_H
#define PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_GENERATOR_H

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/fdr_types.h"
#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

#include <memory>

namespace probfd {
class ProbabilisticTask;
namespace heuristics {
namespace pdbs {

class PatternCollectionInformation;

class PatternCollectionGenerator {
protected:
    mutable utils::LogProxy log;

public:
    explicit PatternCollectionGenerator(const plugins::Options& opts);
    explicit PatternCollectionGenerator(const utils::LogProxy& log);
    virtual ~PatternCollectionGenerator() = default;

    virtual PatternCollectionInformation generate(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) = 0;
};

extern void
add_pattern_collection_generator_options_to_feature(plugins::Feature& feature);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd
#endif // __PATTERN_GENERATOR_H__