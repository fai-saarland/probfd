#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_INCREMENTAL_CANONICAL_PDBS_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_INCREMENTAL_CANONICAL_PDBS_H

#include "pattern_collection_information.h"

#include "../../../../operator_cost.h"
#include "../../../evaluation_result.h"
#include "../../../value_type.h"
#include "../types.h"

#include <memory>

class GlobalState;

namespace probabilistic {
namespace heuristics {
namespace pdbs {

class CombinationStrategy;

namespace pattern_selection {

template <class PDBType>
class IncrementalPPDBs {
    std::shared_ptr<PatternCollection> patterns;
    std::shared_ptr<PPDBCollection<PDBType>> pattern_databases;
    std::shared_ptr<std::vector<PatternSubCollection>> pattern_subcollections;

    std::shared_ptr<SubCollectionFinder> subcollection_finder;

    // The sum of all abstract state sizes of all pdbs in the collection.
    long long size;

    // Adds a PDB for pattern but does not recompute pattern_subcollections.
    void add_pdb_for_pattern(const Pattern& pattern);

    void recompute_pattern_subcollections();

public:
    IncrementalPPDBs(
        const PatternCollection& initial_patterns,
        std::shared_ptr<SubCollectionFinder> subcollection_finder);

    IncrementalPPDBs(
        PatternCollectionInformation<PDBType>& initial_patterns,
        std::shared_ptr<SubCollectionFinder> subcollection_finder);

    // Adds a new PDB to the collection and recomputes pattern_subcollections.
    void add_pdb(const std::shared_ptr<PDBType>& pdb);

    /* Returns a list of pattern subcollections that would be additive to the
       new pattern. */
    std::vector<PatternSubCollection>
    get_pattern_subcollections(const Pattern& new_pattern);

    value_type::value_t get_value(const GlobalState& state) const;

    EvaluationResult evaluate(const GlobalState& state) const;

    /*
      The following method offers a quick dead-end check for the sampling
      procedure of iPDB-hillclimbing. This exists because we can much more
      efficiently test if the canonical heuristic is infinite than
      computing the exact heuristic value.
    */
    bool is_dead_end(const GlobalState& state) const;

    PatternCollectionInformation<PDBType>
    get_pattern_collection_information() const;

    std::shared_ptr<PPDBCollection<PDBType>> get_pattern_databases() const;

    long long get_size() const;
};

using IncrementalExpCostPDBs = IncrementalPPDBs<ExpCostProjection>;
using IncrementalMaxProbPDBs = IncrementalPPDBs<MaxProbProjection>;

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probabilistic
#endif // __INCREMENTAL_CANONICAL_PDBS_H__