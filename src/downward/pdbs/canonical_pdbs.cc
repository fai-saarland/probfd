#include "downward/pdbs/canonical_pdbs.h"

#include "downward/pdbs/pattern_database.h"

#include <algorithm>
#include <cassert>
#include <iostream>
#include <limits>

using namespace std;

namespace downward::pdbs {
CanonicalPDBs::CanonicalPDBs(
    const shared_ptr<PDBCollection>& pdbs,
    const shared_ptr<vector<PatternClique>>& pattern_cliques)
    : pdbs(pdbs)
    , pattern_cliques(pattern_cliques)
{
    assert(pdbs);
    assert(pattern_cliques);
}

int CanonicalPDBs::get_value(const State& state) const
{
    // If we have an empty collection, then pattern_cliques = { \emptyset }.
    assert(!pattern_cliques->empty());
    int max_h = 0;
    vector<int> h_values;
    h_values.reserve(pdbs->size());
    state.unpack();
    for (const shared_ptr<PatternDatabase>& pdb : *pdbs) {
        int h = pdb->get_value(state.get_unpacked_values());
        if (h == numeric_limits<int>::max()) {
            return numeric_limits<int>::max();
        }
        h_values.push_back(h);
    }
    for (const PatternClique& clique : *pattern_cliques) {
        int clique_h = 0;
        for (PatternID pdb_index : clique) {
            clique_h += h_values[pdb_index];
        }
        max_h = max(max_h, clique_h);
    }
    return max_h;
}
} // namespace pdbs
