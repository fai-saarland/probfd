#ifndef PROBFD_PDBS_PATTERN_INFORMATION_H
#define PROBFD_PDBS_PATTERN_INFORMATION_H

#include "probfd/pdbs/types.h"

#include "probfd/heuristics/constant_heuristic.h"

#include "probfd/fdr_types.h"
#include "probfd/task_proxy.h"

#include <memory>

namespace probfd::pdbs {

struct ProbabilityAwarePatternDatabase;

class PatternInformation {
    ProbabilisticTaskProxy task_proxy_;
    std::shared_ptr<FDRSimpleCostFunction> task_cost_function_;
    Pattern pattern_;
    std::shared_ptr<ProbabilityAwarePatternDatabase> pdb_;

    // for creation if missing
    heuristics::BlindEvaluator<StateRank> h;

    void create_pdb_if_missing();

    [[nodiscard]]
    bool information_is_valid() const;

public:
    PatternInformation(
        ProbabilisticTaskProxy task_proxy,
        std::shared_ptr<FDRSimpleCostFunction> task_cost_function,
        Pattern pattern);

    void set_pdb(const std::shared_ptr<ProbabilityAwarePatternDatabase>& pdb);

    [[nodiscard]]
    ProbabilisticTaskProxy get_task_proxy() const
    {
        return task_proxy_;
    }

    [[nodiscard]]
    const Pattern& get_pattern() const;
    std::shared_ptr<ProbabilityAwarePatternDatabase> get_pdb();
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PATTERN_INFORMATION_H
