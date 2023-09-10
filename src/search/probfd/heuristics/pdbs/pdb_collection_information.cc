#include "probfd/heuristics/pdbs/pdb_collection_information.h"

#include <utility>

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {

PDBCollectionInformation::PDBCollectionInformation(
    PPDBCollection pdbs,
    std::shared_ptr<PDBCombinator> pdb_combinator)
    : pdbs_(std::move(pdbs))
    , pdb_combinator_(std::move(pdb_combinator))
{
}

PPDBCollection& PDBCollectionInformation::get_pdbs()
{
    return pdbs_;
}

shared_ptr<PDBCombinator> PDBCollectionInformation::get_pdb_combinator()
{
    return pdb_combinator_;
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd