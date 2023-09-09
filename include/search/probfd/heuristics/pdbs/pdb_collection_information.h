#ifndef PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_INFORMATION_H
#define PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_INFORMATION_H

#include "probfd/heuristics/pdbs/subcollection_finder.h"

#include "probfd/heuristics/pdbs/types.h"

#include "probfd/fdr_types.h"
#include "probfd/task_proxy.h"

#include <memory>

namespace pdbs {
class PDBCollectionInformation;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

class PDBCollectionInformation {
    PPDBCollection pdbs_;
    std::shared_ptr<SubCollectionFinder> subcollection_finder_;
    std::vector<PatternSubCollection> subcollections_;

public:
    PDBCollectionInformation(
        PPDBCollection pdbs,
        std::shared_ptr<SubCollectionFinder> subcollection_finder);

    PDBCollectionInformation(
        PPDBCollection pdbs,
        std::shared_ptr<SubCollectionFinder> subcollection_finder,
        std::vector<PatternSubCollection> subcollections);

    PPDBCollection& get_pdbs();
    std::vector<PatternSubCollection>& get_subcollections();
    std::shared_ptr<SubCollectionFinder> get_subcollection_finder();
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_PDBS_PDB_COLLECTION_INFORMATION_H
