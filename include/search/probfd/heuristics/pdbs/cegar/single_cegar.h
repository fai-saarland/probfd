#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_CEGAR_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_CEGAR_H

#include "probfd/heuristics/pdbs/cegar/flaw.h"

#include "probfd/fdr_types.h"
#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

#include <memory>

namespace utils {
class CountdownTimer;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

class ProjectionFactory;
class FlawGenerator;

struct ProjectionInfo;

class SingleCEGAR {
    // Initial projection factory
    const std::shared_ptr<ProjectionFactory> projection_factory;

    // Flaw generation
    const std::shared_ptr<FlawGenerator> flaw_generator;

    // Log output
    mutable utils::LogProxy log;

public:
    SingleCEGAR(
        std::shared_ptr<ProjectionFactory> projection_factory,
        std::shared_ptr<FlawGenerator> flaw_generator,
        utils::LogProxy log);

    ~SingleCEGAR();

    ProjectionInfo generate_pdb(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction& task_cost_function,
        double max_time);

private:
    void refine(
        ProbabilisticTaskProxy task_proxy,
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
