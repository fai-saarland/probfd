#ifndef PROBFD_HEURISTICS_PDBS_PROBAIBLITY_AWARE_PDB_HEURISTIC_H
#define PROBFD_HEURISTICS_PDBS_PROBAIBLITY_AWARE_PDB_HEURISTIC_H

#include "probfd/heuristics/pdbs/pattern_generator.h"

#include "probfd/heuristics/pdbs/state_rank.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/heuristics/task_dependent_heuristic.h"

#include "utils/printable.h"

#include <memory>
#include <ostream>
#include <vector>

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace probfd {
namespace heuristics {
namespace pdbs {

/**
 * @brief The probability-aware PDB heuristic.
 *
 * Combines multiple projections heuristics. The type of combination is
 * specified by the configuration of the pattern collection generation
 * algorithms (see options).
 */
class ProbabilityAwarePDBHeuristic : public TaskDependentHeuristic {
    std::shared_ptr<std::vector<Pattern>> patterns;
    std::shared_ptr<PPDBCollection> pdbs;
    std::shared_ptr<std::vector<PatternSubCollection>> subcollections;
    std::shared_ptr<SubCollectionFinder> subcollection_finder;

public:
    /**
     * @brief Construct from options.
     */
    explicit ProbabilityAwarePDBHeuristic(const plugins::Options& opts);

    ProbabilityAwarePDBHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<PatternCollectionGenerator> generator,
        double max_time_dominance_pruning,
        utils::LogProxy log);

    EvaluationResult evaluate(const State& state) const override;

public:
    static void add_options_to_feature(plugins::Feature& feature);
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __PROBABILISTIC_PDB_HEURISTIC_H__