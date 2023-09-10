#ifndef PROBFD_HEURISTICS_PDBS_INCREMENTAL_PDB_COMBINATOR_FACTORY_H
#define PROBFD_HEURISTICS_PDBS_INCREMENTAL_PDB_COMBINATOR_FACTORY_H

#include "probfd/heuristics/pdbs/pdb_combinator_factory.h"

#include "probfd/task_proxy.h"

#include <memory>

namespace probfd {
namespace heuristics {
namespace pdbs {

class IncrementalPDBCombinator;

class IncrementalPDBCombinatorFactory : public PDBCombinatorFactory {
public:
    virtual std::unique_ptr<IncrementalPDBCombinator>
    create_incremental_pdb_combinator(ProbabilisticTaskProxy task_proxy) = 0;

    std::unique_ptr<PDBCombinator> create_pdb_combinator(
        ProbabilisticTaskProxy task_proxy,
        PPDBCollection& pdbs) override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_INCREMENTAL_PDB_COMBINATOR_FACTORY_H
