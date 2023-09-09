#include "probfd/heuristics/pdbs/pdb_collection_information.h"

#include "downward/pdbs/pattern_cliques.h"

#include "downward/utils/collections.h"
#include "downward/utils/timer.h"

#include <cassert>
#include <utility>

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {

PDBCollectionInformation::PDBCollectionInformation(
    PPDBCollection pdbs,
    std::shared_ptr<SubCollectionFinder> subcollection_finder)
    : pdbs_(std::move(pdbs))
    , subcollection_finder_(std::move(subcollection_finder))
    , subcollections_(
          this->subcollection_finder_->compute_subcollections(pdbs_))
{
}

PDBCollectionInformation::PDBCollectionInformation(
    PPDBCollection pdbs,
    std::shared_ptr<SubCollectionFinder> subcollection_finder,
    std::vector<PatternSubCollection> subcollections)
    : pdbs_(std::move(pdbs))
    , subcollection_finder_(std::move(subcollection_finder))
    , subcollections_(std::move(subcollections))
{
}

PPDBCollection& PDBCollectionInformation::get_pdbs()
{
    return pdbs_;
}

vector<PatternSubCollection>& PDBCollectionInformation::get_subcollections()
{
    return subcollections_;
}

shared_ptr<SubCollectionFinder>
PDBCollectionInformation::get_subcollection_finder()
{
    return subcollection_finder_;
}

} // namespace pdbs
} // namespace heuristics
} // namespace probfd