#include "incremental_canonical_pdbs.h"

#include "../maxprob_projection.h"
#include "../multiplicative_mppdbs.h"
#include "pattern_collection_information.h"


#include "../../../../../global_state.h"
#include "../../../../../pdbs/pattern_cliques.h"
#include "../../../../../pdbs/pattern_collection_information.h"

#include <limits>

using namespace std;

namespace probabilistic {
namespace pdbs {
namespace maxprob {
namespace pattern_selection {

IncrementalCanonicalPDBs::IncrementalCanonicalPDBs(
    const PatternCollection& initial_patterns)
    : patterns(new PatternCollection(
          initial_patterns.begin(),
          initial_patterns.end()))
    , pattern_databases(make_shared<MaxProbPDBCollection>())
    , pattern_cliques(nullptr)
    , size(0)
{
    pattern_databases->reserve(patterns->size());
    for (const Pattern& pattern : *patterns)
        add_pdb_for_pattern(pattern);
    are_additive = ::pdbs::compute_additive_vars();
    recompute_pattern_cliques();
}

static unsigned long long
compute_total_pdb_size(const MaxProbPDBCollection& pdbs)
{
    unsigned long long size = 0;

    for (const auto& pdb : pdbs) {
        size += pdb->num_states();
    }

    return size;
}

IncrementalCanonicalPDBs::IncrementalCanonicalPDBs(
    PatternCollectionInformation& initial_patterns)
    : patterns(initial_patterns.get_patterns())
    , pattern_databases(initial_patterns.get_pdbs())
    , pattern_cliques(initial_patterns.get_pattern_cliques())
    , size(compute_total_pdb_size(*pattern_databases))
{
    // TODO use additivity strategy instead
    are_additive = ::pdbs::compute_additive_vars();
}

void IncrementalCanonicalPDBs::add_pdb_for_pattern(const Pattern& pattern)
{
    pattern_databases->emplace_back(new MaxProbProjection(pattern));
    size += pattern_databases->back()->num_states();
}

void IncrementalCanonicalPDBs::add_pdb(const shared_ptr<MaxProbProjection>& pdb)
{
    patterns->push_back(pdb->get_pattern());
    pattern_databases->push_back(pdb);
    size += pattern_databases->back()->num_states();
    recompute_pattern_cliques();
}

void IncrementalCanonicalPDBs::recompute_pattern_cliques()
{
    pattern_cliques = ::pdbs::compute_pattern_cliques(*patterns, are_additive);
}

vector<PatternClique>
IncrementalCanonicalPDBs::get_pattern_cliques(const Pattern& new_pattern)
{
    return ::pdbs::compute_pattern_cliques_with_pattern(
        *patterns,
        *pattern_cliques,
        new_pattern,
        are_additive);
}

value_type::value_t
IncrementalCanonicalPDBs::get_value(const GlobalState& state) const
{
    MultiplicativeMaxProbPDBs canonical_pdbs(
        pattern_databases,
        pattern_cliques);
    return static_cast<value_type::value_t>(canonical_pdbs.evaluate(state));
}

bool IncrementalCanonicalPDBs::is_dead_end(const GlobalState& state) const
{
    for (const shared_ptr<MaxProbProjection>& pdb : *pattern_databases)
        if (pdb->is_dead_end(state))
            return true;
    return false;
}

PatternCollectionInformation
IncrementalCanonicalPDBs::get_pattern_collection_information() const
{
    PatternCollectionInformation result(patterns);
    result.set_pdbs(pattern_databases);
    result.set_pattern_cliques(pattern_cliques);
    return result;
}

} // namespace pattern_selection
} // namespace maxprob
} // namespace pdbs
} // namespace probabilistic