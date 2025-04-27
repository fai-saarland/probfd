#ifndef PDBS_ZERO_ONE_PDBS_HEURISTIC_H
#define PDBS_ZERO_ONE_PDBS_HEURISTIC_H

#include "downward/pdbs/pattern_generator.h"
#include "downward/pdbs/zero_one_pdbs.h"

#include "downward/heuristic.h"

namespace downward::pdbs {
class PatternDatabase;

class ZeroOnePDBsHeuristic : public Heuristic {
    ZeroOnePDBs zero_one_pdbs;

protected:
    int compute_heuristic(const State& ancestor_state) override;

public:
    ZeroOnePDBsHeuristic(
        const std::shared_ptr<PatternCollectionGenerator>& patterns,
        SharedAbstractTask original_task,
        TaskTransformationResult transformation_result,
        bool cache_estimates,
        const std::string& name,
        utils::Verbosity verbosity);

    ZeroOnePDBsHeuristic(
        const std::shared_ptr<PatternCollectionGenerator>& patterns,
        SharedAbstractTask original_task,
        const std::shared_ptr<TaskTransformation>& transformation,
        bool cache_estimates,
        const std::string& name,
        utils::Verbosity verbosity);
};
} // namespace pdbs

#endif
