#include "incremental_canonical_pdbs.h"

#include "canonical_pdbs.h"
#include "pattern_database.h"
#include "../global_state.h"
#include <limits>

using namespace std;

namespace pdbs {
IncrementalCanonicalPDBs::IncrementalCanonicalPDBs(
    OperatorCost cost_type,
    const PatternCollection &intitial_patterns)
    : patterns(make_shared<PatternCollection>(intitial_patterns.begin(),
                                              intitial_patterns.end())),
      pattern_databases(make_shared<PDBCollection>()),
      pattern_cliques(nullptr),
      size(0),
      cost_type(cost_type) {
    pattern_databases->reserve(patterns->size());
    for (const Pattern &pattern : *patterns)
        add_pdb_for_pattern(pattern);
    are_additive = compute_additive_vars();
    recompute_pattern_cliques();
}

static unsigned long long compute_total_pdb_size(
    const PDBCollection& pdbs)
{
    unsigned long long size = 0;

    for (const auto& pdb : pdbs) {
        size += pdb->get_size();
    }

    return size;
}

IncrementalCanonicalPDBs::IncrementalCanonicalPDBs(
    OperatorCost cost_type,
    PatternCollectionInformation& info)
    : patterns(info.get_patterns())
    , pattern_databases(info.get_pdbs())
    , pattern_cliques(info.get_pattern_cliques())
    , size(compute_total_pdb_size(*pattern_databases))
    , cost_type(cost_type)
{
    // TODO use additivity strategy instead
    are_additive = ::pdbs::compute_additive_vars();
}

void IncrementalCanonicalPDBs::add_pdb_for_pattern(const Pattern &pattern) {
    pattern_databases->push_back(make_shared<PatternDatabase>(pattern, cost_type));
    size += pattern_databases->back()->get_size();
}

void IncrementalCanonicalPDBs::add_pdb(const shared_ptr<PatternDatabase> &pdb) {
    patterns->push_back(pdb->get_pattern());
    pattern_databases->push_back(pdb);
    size += pattern_databases->back()->get_size();
    recompute_pattern_cliques();
}

void IncrementalCanonicalPDBs::recompute_pattern_cliques() {
    pattern_cliques = compute_pattern_cliques(*patterns,
                                              are_additive);
}

vector<PatternClique> IncrementalCanonicalPDBs::get_pattern_cliques(
    const Pattern &new_pattern) {
    return pdbs::compute_pattern_cliques_with_pattern(
        *patterns, *pattern_cliques, new_pattern, are_additive);
}

int IncrementalCanonicalPDBs::get_value(const GlobalState &state) const {
    CanonicalPDBs canonical_pdbs(pattern_databases, pattern_cliques);
    return canonical_pdbs.get_value(state);
}

bool IncrementalCanonicalPDBs::is_dead_end(const GlobalState &state) const {
    for (const shared_ptr<PatternDatabase> &pdb : *pattern_databases)
        if (pdb->get_value(state) == numeric_limits<int>::max())
            return true;
    return false;
}

PatternCollectionInformation
IncrementalCanonicalPDBs::get_pattern_collection_information() const {
    PatternCollectionInformation result(cost_type, patterns);
    result.set_pdbs(pattern_databases);
    result.set_pattern_cliques(pattern_cliques);
    return result;
}
}
