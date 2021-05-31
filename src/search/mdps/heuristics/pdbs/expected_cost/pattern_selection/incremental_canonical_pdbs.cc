#include "incremental_canonical_pdbs.h"

#include "pattern_collection_information.h"
#include "../expcost_projection.h"
#include "../additive_ecpdbs.h"

#include "../../../../../global_state.h"
#include "../../../../../pdbs/pattern_cliques.h"
#include "../../../../../pdbs/pattern_collection_information.h"

#include <limits>

using namespace std;

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

IncrementalCanonicalPDBs::IncrementalCanonicalPDBs(
    const PatternCollection &initial_patterns)
    : patterns(new PatternCollection(
          initial_patterns.begin(), initial_patterns.end())),
      pattern_databases(make_shared<ExpCostPDBCollection>()),
      pattern_cliques(nullptr),
      size(0)
{
    pattern_databases->reserve(patterns->size());
    for (const Pattern &pattern : *patterns)
        add_pdb_for_pattern(pattern);
    are_additive = ::pdbs::compute_additive_vars();
    recompute_pattern_cliques();
}

void IncrementalCanonicalPDBs::add_pdb_for_pattern(const Pattern &pattern) {
    pattern_databases->emplace_back(new ExpCostProjection(pattern));
    size += pattern_databases->back()->num_states();
}

void IncrementalCanonicalPDBs::add_pdb(
    const shared_ptr<ExpCostProjection> &pdb)
{
    patterns->push_back(pdb->get_pattern());
    pattern_databases->push_back(pdb);
    size += pattern_databases->back()->num_states();
    recompute_pattern_cliques();
}

void IncrementalCanonicalPDBs::recompute_pattern_cliques() {
    pattern_cliques = ::pdbs::compute_pattern_cliques(*patterns, are_additive);
}

vector<PatternClique> IncrementalCanonicalPDBs::get_pattern_cliques(
    const Pattern &new_pattern) {
    return ::pdbs::compute_pattern_cliques_with_pattern(
        *patterns, *pattern_cliques, new_pattern, are_additive);
}

value_type::value_t
IncrementalCanonicalPDBs::get_value(const GlobalState &state) const {
    AdditiveExpectedCostPDBs canonical_pdbs(pattern_databases, pattern_cliques);
    return static_cast<value_type::value_t>(canonical_pdbs.evaluate(state));
}

bool IncrementalCanonicalPDBs::is_dead_end(const GlobalState &state) const {
    for (const shared_ptr<ExpCostProjection> &pdb : *pattern_databases)
        if (pdb->get_value(state) == numeric_limits<value_type::value_t>::max())
            return true;
    return false;
}

PatternCollectionInformation
IncrementalCanonicalPDBs::get_pattern_collection_information() const {
    PatternCollectionInformation result(patterns);
    result.set_pdbs(pattern_databases);
    result.set_pattern_cliques(pattern_cliques);
    return result;
}

}
}
}