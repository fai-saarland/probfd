#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_SINGLE_CEGAR_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_SINGLE_CEGAR_H

#include "probfd/heuristics/pdbs/evaluators.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/task_proxy.h"

#include "downward/utils/logging.h"

namespace utils {
class CountdownTimer;
class RandomNumberGenerator;
} // namespace utils

namespace probfd {
namespace heuristics {
namespace pdbs {

struct ProjectionInfo;

namespace cegar {

class FlawGenerator;

void run_cegar_refinement_loop(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction& task_cost_function,
    ProjectionInfo& pdb_info,
    FlawGenerator& flaw_generator,
    utils::RandomNumberGenerator& rng,
    utils::LogProxy log,
    utils::CountdownTimer& timer);

extern void add_cegar_wildcard_option_to_feature(plugins::Feature& feature);

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_CEGAR_SINGLE_CEGAR_H
