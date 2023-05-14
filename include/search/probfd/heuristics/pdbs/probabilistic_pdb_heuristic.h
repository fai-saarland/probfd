#ifndef PROBFD_HEURISTICS_PDBS_PROBABILISTIC_PDB_HEURISTIC_H
#define PROBFD_HEURISTICS_PDBS_PROBABILISTIC_PDB_HEURISTIC_H

#include "probfd/heuristics/pdbs/pattern_generator.h"

#include "probfd/heuristics/pdbs/state_rank.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/heuristics/task_dependent_heuristic.h"

#include "utils/printable.h"

#include <memory>
#include <ostream>
#include <vector>

namespace legacy {
class GlobalState;
}

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {
namespace heuristics {
namespace pdbs {

/**
 * @brief Additive Expected-Cost PDB heuristic.
 */
class ProbabilisticPDBHeuristic : public TaskDependentHeuristic {
    std::shared_ptr<std::vector<Pattern>> patterns;
    std::shared_ptr<PPDBCollection> pdbs;
    std::shared_ptr<std::vector<PatternSubCollection>> subcollections;
    std::shared_ptr<SubCollectionFinder> subcollection_finder;

public:
    /**
     * @brief Construct from options.
     */
    explicit ProbabilisticPDBHeuristic(const options::Options& opts);

    ProbabilisticPDBHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<PatternCollectionGenerator> generator,
        double max_time_dominance_pruning,
        utils::LogProxy log);

    EvaluationResult evaluate(const State& state) const override;

public:
    static void add_options_to_parser(options::OptionParser& parser);
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __PROBABILISTIC_PDB_HEURISTIC_H__