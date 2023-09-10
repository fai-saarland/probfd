#ifndef PROBFD_HEURISTICS_PDBS_PDB_COMBINATOR_FACTORY_H
#define PROBFD_HEURISTICS_PDBS_PDB_COMBINATOR_FACTORY_H

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/task_proxy.h"

#include <memory>

namespace probfd {
namespace heuristics {
namespace pdbs {

class PDBCombinator;

class PDBCombinatorFactory {
public:
    virtual ~PDBCombinatorFactory() = default;

    virtual std::unique_ptr<PDBCombinator> create_pdb_combinator(
        ProbabilisticTaskProxy task_proxy,
        PPDBCollection& pdbs) = 0;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_PDB_COMBINATOR_FACTORY_H
