#ifndef PROBFD_PDBS_CEGAR_SINGLE_CEGAR_H
#define PROBFD_PDBS_CEGAR_SINGLE_CEGAR_H

#include "probfd/pdbs/state_ranking_function.h"
#include "probfd/pdbs/types.h"

#include "probfd/fdr_types.h"
#include "probfd/probabilistic_task.h"
#include "probfd/value_type.h"

#include <memory>
#include <unordered_set>
#include <vector>

// Forward Declarations
namespace downward {
class VariableSpace;
}

namespace downward::utils {
class CountdownTimer;
class LogProxy;
class RandomNumberGenerator;
} // namespace utils

namespace probfd::pdbs {
class ProjectionStateSpace;
struct ProjectionTransformation;
struct ProbabilityAwarePatternDatabase;
} // namespace probfd::pdbs

namespace probfd::pdbs::cegar {
struct Flaw;
class FlawFindingStrategy;
} // namespace probfd::pdbs::cegar

namespace probfd::pdbs::cegar {

extern void run_cegar_loop(
    ProjectionTransformation& transformation,
    const SharedProbabilisticTask& task,
    const downward::State& initial_state,
    value_t convergence_epsilon,
    FlawFindingStrategy& flaw_strategy,
    std::unordered_set<int> blacklisted_variables,
    int max_pdb_size,
    downward::utils::RandomNumberGenerator& rng,
    bool wildcard,
    double max_time,
    downward::utils::LogProxy log);

} // namespace probfd::pdbs::cegar

#endif // PROBFD_PDBS_CEGAR_SINGLE_CEGAR_H
