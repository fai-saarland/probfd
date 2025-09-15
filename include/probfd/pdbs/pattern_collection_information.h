#ifndef PROBFD_PDBS_PATTERN_COLLECTION_INFORMATION_H
#define PROBFD_PDBS_PATTERN_COLLECTION_INFORMATION_H

#include "probfd/pdbs/subcollection_finder.h"
#include "probfd/pdbs/types.h"

#include "probfd/heuristics/constant_heuristic.h"

#include "probfd/fdr_types.h"

#include <memory>
#include <vector>

namespace downward::pdbs {
class PatternCollectionInformation;
}

namespace probfd::pdbs {

/*
  This class contains everything we know about a pattern collection. It will
  always contain patterns, but can also contain the computed PDBs and
  additive subsets of the PDBs. If one of the latter is not available, then
  this information is created when it is requested.

  TODO: this should probably re-use PatternInformation and it could also act
  as an interface for ownership transfer rather than sharing it.
*/
class PatternCollectionInformation {
    SharedProbabilisticTask task_;

    PatternCollection patterns_;
    PPDBCollection pdbs_;
    std::vector<PatternSubCollection> subcollections_;

    std::shared_ptr<SubCollectionFinder> subcollection_finder_;

    // for creation if missing
    heuristics::BlindHeuristic<StateRank> h;

    void create_pdbs_if_missing();
    void create_pattern_cliques_if_missing();

public:
    PatternCollectionInformation(
        SharedProbabilisticTask task,
        downward::pdbs::PatternCollectionInformation det_info,
        std::shared_ptr<SubCollectionFinder> subcollection_finder);

    PatternCollectionInformation(
        SharedProbabilisticTask task,
        PatternCollection patterns);

    PatternCollectionInformation(
        SharedProbabilisticTask task,
        PatternCollection patterns,
        std::shared_ptr<SubCollectionFinder> subcollection_finder);

    void set_pdbs(PPDBCollection pdbs);
    void set_subcollections(std::vector<PatternSubCollection> subcollections);

    [[nodiscard]]
    const PatternCollection& get_patterns() const;
    PPDBCollection& get_pdbs();
    std::vector<PatternSubCollection>& get_subcollections();
    std::shared_ptr<SubCollectionFinder> get_subcollection_finder();
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PATTERN_COLLECTION_INFORMATION_H
