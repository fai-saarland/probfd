#ifndef PROBFD_PDBS_PATTERN_INFORMATION_H
#define PROBFD_PDBS_PATTERN_INFORMATION_H

#include "probfd/pdbs/types.h"

#include "probfd/heuristics/constant_heuristic.h"

#include "probfd/fdr_types.h"

#include <memory>

namespace probfd::pdbs {

struct ProbabilityAwarePatternDatabase;

class PatternInformation {
    std::shared_ptr<ProbabilisticTask> task_;
    Pattern pattern_;
    std::shared_ptr<ProbabilityAwarePatternDatabase> pdb_;

    // for creation if missing
    heuristics::BlindEvaluator<StateRank> h;

    void create_pdb_if_missing();

    [[nodiscard]]
    bool information_is_valid() const;

public:
    PatternInformation(
        std::shared_ptr<ProbabilisticTask> task,
        Pattern pattern);

    void set_pdb(const std::shared_ptr<ProbabilityAwarePatternDatabase>& pdb);

    [[nodiscard]]
    const ProbabilisticTask& get_task() const
    {
        return *task_;
    }

    [[nodiscard]]
    const Pattern& get_pattern() const;
    std::shared_ptr<ProbabilityAwarePatternDatabase> get_pdb();
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PATTERN_INFORMATION_H
