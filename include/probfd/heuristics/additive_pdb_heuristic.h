#ifndef PROBFD_HEURISTICS_ADDITIVE_PDB_HEURISTIC_H
#define PROBFD_HEURISTICS_ADDITIVE_PDB_HEURISTIC_H

#include "probfd/fdr_types.h"
#include "probfd/heuristic.h"

#include <vector>

namespace probfd::pdbs {
struct ProbabilityAwarePatternDatabase;
} // namespace probfd::pdbs

namespace probfd::heuristics {

class AdditivePDBHeuristic : public FDRHeuristic {
    const std::vector<pdbs::ProbabilityAwarePatternDatabase> pdbs_;

public:
    explicit AdditivePDBHeuristic(
        std::vector<pdbs::ProbabilityAwarePatternDatabase> pdbs);

    ~AdditivePDBHeuristic() override;

    value_t evaluate(const downward::State& state) const override;
};

} // namespace probfd::heuristics

#endif // PROBFD_HEURISTICS_ADDITIVE_PDB_HEURISTIC_H
