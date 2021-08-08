#pragma once

#include "pattern_collection_information.h"

#include "../../../../../operator_cost.h"
#include "../../../../value_type.h"
#include "../../types.h"

#include <memory>

class GlobalState;

namespace probabilistic {
namespace pdbs {
namespace expected_cost {
namespace pattern_selection {

class IncrementalCanonicalPDBs {
    std::shared_ptr<PatternCollection> patterns;
    std::shared_ptr<ExpCostPDBCollection> pattern_databases;
    std::shared_ptr<std::vector<PatternClique>> pattern_cliques;

    // A pair of variables is additive if no operator has an effect on both.
    VariableAdditivity are_additive;

    // The sum of all abstract state sizes of all pdbs in the collection.
    long long size;

    // Adds a PDB for pattern but does not recompute pattern_cliques.
    void add_pdb_for_pattern(const Pattern& pattern);

    void recompute_pattern_cliques();

public:
    IncrementalCanonicalPDBs(const PatternCollection& initial_patterns);

    IncrementalCanonicalPDBs(PatternCollectionInformation& initial_patterns);

    virtual ~IncrementalCanonicalPDBs() = default;

    // Adds a new PDB to the collection and recomputes pattern_cliques.
    void add_pdb(const std::shared_ptr<ExpCostProjection>& pdb);

    /* Returns a list of pattern cliques that would be additive to the new
       pattern. Detailed documentation in max_additive_pdb_sets.h */
    std::vector<PatternClique> get_pattern_cliques(const Pattern& new_pattern);

    value_type::value_t get_value(const GlobalState& state) const;

    /*
      The following method offers a quick dead-end check for the sampling
      procedure of iPDB-hillclimbing. This exists because we can much more
      efficiently test if the canonical heuristic is infinite than
      computing the exact heuristic value.
    */
    bool is_dead_end(const GlobalState& state) const;

    PatternCollectionInformation get_pattern_collection_information() const;

    std::shared_ptr<ExpCostPDBCollection> get_pattern_databases() const
    {
        return pattern_databases;
    }

    long long get_size() const { return size; }
};

} // namespace pattern_selection
} // namespace expected_cost
} // namespace pdbs
} // namespace probabilistic