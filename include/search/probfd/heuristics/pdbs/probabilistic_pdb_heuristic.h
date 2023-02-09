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
template <class PDBType>
class ProbabilisticPDBHeuristic : public TaskDependentHeuristic {
    struct Statistics {
        double generator_time = 0.0;
        double dominance_pruning_time = 0.0;
        double construction_time = 0.0;

        size_t pdbs = 0;
        size_t variables = 0;
        size_t abstract_states = 0;
        size_t largest_pattern = 0;

        size_t num_subcollections = 0;
        size_t total_subcollections_size = 0;

        // Run-time statistics (expensive)
        // TODO

        void print_construction_info(std::ostream& out) const;
        void print(std::ostream& out) const;
    };

    Statistics statistics_;

    std::shared_ptr<utils::Printable> generator_report;

    std::shared_ptr<std::vector<Pattern>> patterns;
    std::shared_ptr<PPDBCollection<PDBType>> pdbs;
    std::shared_ptr<std::vector<PatternSubCollection>> subcollections;

public:
    /**
     * @brief Construct from options.
     *
     * @param opts - The following options are available:
     * + \em patterns - The generator used to generate the initial pattern
     * collection. By default, uses a systematic pattern generation algorithm
     * with size bound 2.
     * + \em max_time_dominance_pruning - The maximum time allowed for
     * dominance pruning. A value of zero disables dominance pruning. By
     * default, this option is disabled.
     * + \em time_limit - The maximal time allowed to construct the databases
     * for the generated pattern collection. A value of zero disables the time
     * limit. by default, no time limit is imposed.
     * + \em max_states - The maximal number of abstract states allowed. By
     * default, no restrictions are imposed.
     * + \em dump_projections - If true, dump the projection with graphviz.
     * False by default.
     * + \em additive - Specifies whether the additivity criterion should be
     * used. True by default.
     */
    explicit ProbabilisticPDBHeuristic(const options::Options& opts);

    ProbabilisticPDBHeuristic(
        std::shared_ptr<ProbabilisticTask> task,
        std::shared_ptr<PatternCollectionGenerator<PDBType>> generator,
        double max_time_dominance_pruning);

    void print_statistics() const override;

    EvaluationResult evaluate(const State& state) const override;

public:
    static void add_options_to_parser(options::OptionParser& parser);
};

using MaxProbPDBHeuristic = ProbabilisticPDBHeuristic<MaxProbPatternDatabase>;
using ExpCostPDBHeuristic = ProbabilisticPDBHeuristic<SSPPatternDatabase>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __PROBABILISTIC_PDB_HEURISTIC_H__