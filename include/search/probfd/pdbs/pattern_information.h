#ifndef PROBFD_PDBS_PATTERN_INFORMATION_H
#define PROBFD_PDBS_PATTERN_INFORMATION_H

#include "probfd/pdbs/types.h"

#include "probfd/fdr_types.h"
#include "probfd/task_proxy.h"

#include <memory>

namespace probfd {
namespace pdbs {

class ProbabilityAwarePatternDatabase;

class PatternInformation {
    ProbabilisticTaskProxy task_proxy;
    FDRSimpleCostFunction* task_cost_function;
    Pattern pattern;
    std::shared_ptr<ProbabilityAwarePatternDatabase> pdb;

    void create_pdb_if_missing();

    bool information_is_valid() const;

public:
    PatternInformation(
        ProbabilisticTaskProxy task_proxy,
        FDRSimpleCostFunction* task_cost_function,
        Pattern pattern);

    void set_pdb(const std::shared_ptr<ProbabilityAwarePatternDatabase>& pdb);

    ProbabilisticTaskProxy get_task_proxy() const { return task_proxy; }

    const Pattern& get_pattern() const;
    std::shared_ptr<ProbabilityAwarePatternDatabase> get_pdb();
};

} // namespace pdbs
} // namespace probfd

#endif // PROBFD_PDBS_PATTERN_INFORMATION_H
