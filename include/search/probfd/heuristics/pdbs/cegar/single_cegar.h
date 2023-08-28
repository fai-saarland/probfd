#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_CEGAR_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_CEGAR_H

#include "probfd/heuristics/pdbs/evaluators.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/heuristics/pdbs/cegar/flaw.h"
#include "probfd/heuristics/pdbs/cegar/projection_info.h"

#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace utils {
class CountdownTimer;
class RandomNumberGenerator;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace pdbs {

class StateRankingFunction;
class ProjectionStateSpace;
class IncrementalValueTableEvaluator;

namespace cegar {

class FlawGenerator;
class ProjectionFactory;

class SingleCEGAR {
    // Flaw generation
    const std::shared_ptr<FlawGenerator> flaw_generator;

    // Initial projection factory
    const std::shared_ptr<ProjectionFactory> projection_factory;

    // Log output
    mutable utils::LogProxy log;

public:
    SingleCEGAR(
        std::shared_ptr<FlawGenerator> flaw_generator,
        std::shared_ptr<ProjectionFactory> projection_factory,
        utils::LogProxy log);

    ~SingleCEGAR();

    ProjectionInfo generate_pdb(
        const ProbabilisticTaskProxy& task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        double max_time);

private:
    void refine(
        const ProbabilisticTaskProxy& task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        ProjectionInfo& projection_info,
        Flaw flaw,
        utils::CountdownTimer& timer);
};

extern void add_cegar_wildcard_option_to_feature(plugins::Feature& feature);

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PDBS_PATTERN_COLLECTION_GENERATOR_CEGAR_H
