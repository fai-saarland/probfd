#ifndef PROBFD_PDBS_PATTERN_INFORMATION_H
#define PROBFD_PDBS_PATTERN_INFORMATION_H

#include "probfd/pdbs/types.h"

#include "probfd/fdr_types.h"
#include "probfd/task_proxy.h"

#include <memory>

namespace probfd::pdbs {

class ProbabilityAwarePatternDatabase;

class PatternInformation {
    ProbabilisticTaskProxy task_proxy_;
    FDRSimpleCostFunction* task_cost_function_;
    Pattern pattern_;
    std::shared_ptr<ProbabilityAwarePatternDatabase> pdb_;

    void create_pdb_if_missing();

    [[nodiscard]]
    bool information_is_valid() const;

public:
    PatternInformation(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction* task_cost_function,
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
