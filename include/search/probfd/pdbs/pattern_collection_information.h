#ifndef PROBFD_PDBS_PATTERN_COLLECTION_INFORMATION_H
#define PROBFD_PDBS_PATTERN_COLLECTION_INFORMATION_H

#include "probfd/pdbs/subcollection_finder.h"
#include "probfd/pdbs/types.h"

#include "probfd/fdr_types.h"
#include "probfd/task_proxy.h"

#include <memory>
#include <vector>

namespace pdbs {
class PatternCollectionInformation;
}

namespace probfd::pdbs {

/*
  This class contains everything we know about a pattern collection. It will
  always contain patterns, but can also contain the computed PDBs and
  additive subsets of the PDBs. If one of the latter is not available, then
  this information is created when it is requested.
  Ownership of the information is shared between the creators of this class
  (usually PatternCollectionGenerators), the class itself, and its users
  (consumers of pattern collections like heuristics).

  TODO: this should probably re-use PatternInformation and it could also act
  as an interface for ownership transfer rather than sharing it.
*/
class PatternCollectionInformation {
    ProbabilisticTaskProxy task_proxy_;
    std::shared_ptr<FDRCostFunction> task_cost_function_;

    std::shared_ptr<PatternCollection> patterns_;
    std::shared_ptr<PPDBCollection> pdbs_;
    std::shared_ptr<std::vector<PatternSubCollection>> subcollections_;

    std::shared_ptr<SubCollectionFinder> subcollection_finder_;

    void create_pdbs_if_missing();
    void create_pattern_cliques_if_missing();

    [[nodiscard]]
    bool information_is_valid() const;

public:
    PatternCollectionInformation(
        const ProbabilisticTaskProxy& task_proxy,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        ::pdbs::PatternCollectionInformation det_info,
        std::shared_ptr<SubCollectionFinder> subcollection_finder);

    PatternCollectionInformation(
        const ProbabilisticTaskProxy& task_proxy,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        std::shared_ptr<PatternCollection> patterns);

    PatternCollectionInformation(
        const ProbabilisticTaskProxy& task_proxy,
        std::shared_ptr<FDRCostFunction> task_cost_function,
        std::shared_ptr<PatternCollection> patterns,
        std::shared_ptr<SubCollectionFinder> subcollection_finder);

    void set_pdbs(const std::shared_ptr<PPDBCollection>& pdbs);
    void
    set_subcollections(const std::shared_ptr<std::vector<PatternSubCollection>>&
                           subcollections);

    [[nodiscard]]
    std::shared_ptr<PatternCollection> get_patterns() const;
    std::shared_ptr<PPDBCollection> get_pdbs();
    std::shared_ptr<std::vector<PatternSubCollection>> get_subcollections();
    std::shared_ptr<SubCollectionFinder> get_subcollection_finder();
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PATTERN_COLLECTION_INFORMATION_H
