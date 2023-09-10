#ifndef PROBFD_HEURISTICS_PDBS_INCREMENTAL_PDB_COMBINATOR_H
#define PROBFD_HEURISTICS_PDBS_INCREMENTAL_PDB_COMBINATOR_H

#include "probfd/heuristics/pdbs/pdb_combinator.h"
#include "probfd/heuristics/pdbs/types.h"

#include "probfd/value_type.h"

#include <memory>
#include <vector>

class State;

namespace probfd {
namespace heuristics {
namespace pdbs {

class ProbabilityAwarePatternDatabase;

class IncrementalPDBCombinator : public PDBCombinator {
public:
    virtual int count_improvements_if_added(
        const PPDBCollection& pdbs,
        const ProbabilityAwarePatternDatabase& new_pdb,
        const std::vector<State>& states,
        value_t termination_cost) = 0;

    virtual void update(const PPDBCollection&) = 0;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_INCREMENTAL_PDB_COMBINATOR_H
