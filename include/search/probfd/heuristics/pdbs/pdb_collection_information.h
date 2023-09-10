#ifndef PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_INFORMATION_H
#define PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_INFORMATION_H

#include "probfd/heuristics/pdbs/pdb_combinator.h"

#include "probfd/heuristics/pdbs/types.h"

#include <memory>

namespace probfd {
namespace heuristics {
namespace pdbs {

class PDBCollectionInformation {
    PPDBCollection pdbs_;
    std::shared_ptr<PDBCombinator> pdb_combinator_;

public:
    PDBCollectionInformation(
        PPDBCollection pdbs,
        std::shared_ptr<PDBCombinator> pdb_combinator);

    PPDBCollection& get_pdbs();
    std::shared_ptr<PDBCombinator> get_pdb_combinator();
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_INFORMATION_H
