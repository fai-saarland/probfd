#include "probfd/heuristics/pdbs/pattern_selection/incremental_canonical_pdbs.h"
#include "probfd/heuristics/pdbs/pattern_selection/pattern_collection_information.h"

#include "probfd/heuristics/pdbs/subcollections/max_orthogonal_finder.h"

#include "probfd/heuristics/pdbs/expcost_projection.h"
#include "probfd/heuristics/pdbs/maxprob_projection.h"
#include "probfd/heuristics/pdbs/utils.h"

#include "pdbs/pattern_collection_information.h"

#include "legacy/global_state.h"

#include <limits>

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

template <class PDBType>
unsigned long long compute_total_pdb_size(const std::vector<PDBType>& pdbs)
{
    unsigned long long size = 0;

    for (const auto& pdb : pdbs) {
        size += pdb->num_states();
    }

    return size;
}

template <class PDBType>
IncrementalPPDBs<PDBType>::IncrementalPPDBs(
    const PatternCollection& initial_patterns,
    std::shared_ptr<SubCollectionFinder> subcollection_finder)
    : patterns(new PatternCollection(
          initial_patterns.begin(),
          initial_patterns.end()))
    , pattern_databases(make_shared<PPDBCollection<PDBType>>())
    , pattern_subcollections(nullptr)
    , subcollection_finder(subcollection_finder)
    , size(0)
{
    pattern_databases->reserve(patterns->size());
    for (const Pattern& pattern : *patterns)
        add_pdb_for_pattern(pattern);
    recompute_pattern_subcollections();
}

template <class PDBType>
IncrementalPPDBs<PDBType>::IncrementalPPDBs(
    PatternCollectionInformation<PDBType>& initial_patterns,
    std::shared_ptr<SubCollectionFinder> subcollection_finder)
    : patterns(initial_patterns.get_patterns())
    , pattern_databases(initial_patterns.get_pdbs())
    , pattern_subcollections(initial_patterns.get_subcollections())
    , subcollection_finder(subcollection_finder)
    , size(compute_total_pdb_size(*pattern_databases))
{
}

template <class PDBType>
void IncrementalPPDBs<PDBType>::add_pdb_for_pattern(const Pattern& pattern)
{
    pattern_databases->emplace_back(new PDBType(pattern));
    size += pattern_databases->back()->num_states();
}

template <class PDBType>
void IncrementalPPDBs<PDBType>::add_pdb(const shared_ptr<PDBType>& pdb)
{
    patterns->push_back(pdb->get_pattern());
    pattern_databases->push_back(pdb);
    size += pattern_databases->back()->num_states();
    recompute_pattern_subcollections();
}

template <class PDBType>
void IncrementalPPDBs<PDBType>::recompute_pattern_subcollections()
{
    pattern_subcollections =
        subcollection_finder->compute_subcollections(*patterns);
}

template <class PDBType>
vector<PatternSubCollection>
IncrementalPPDBs<PDBType>::get_pattern_subcollections(
    const Pattern& new_pattern)
{
    return subcollection_finder->compute_subcollections_with_pattern(
        *patterns,
        *pattern_subcollections,
        new_pattern);
}

template <class PDBType>
value_type::value_t
IncrementalPPDBs<PDBType>::get_value(const legacy::GlobalState& state) const
{
    return evaluate(state).get_estimate();
}

template <class PDBType>
EvaluationResult
IncrementalPPDBs<PDBType>::evaluate(const legacy::GlobalState& state) const
{
    return heuristics::pdbs::evaluate<PDBType>(
        *pattern_databases,
        *pattern_subcollections,
        state);
}

template <class PDBType>
bool IncrementalPPDBs<PDBType>::is_dead_end(
    const legacy::GlobalState& state) const
{
    for (const auto& pdb : *pattern_databases) {
        if (pdb->evaluate(state).is_unsolvable()) {
            return true;
        }
    }

    return false;
}

template <class PDBType>
PatternCollectionInformation<PDBType>
IncrementalPPDBs<PDBType>::get_pattern_collection_information() const
{
    PatternCollectionInformation<PDBType> result(patterns);
    result.set_pdbs(pattern_databases);
    result.set_subcollections(pattern_subcollections);
    return result;
}

template <class PDBType>
std::shared_ptr<PPDBCollection<PDBType>>
IncrementalPPDBs<PDBType>::get_pattern_databases() const
{
    return pattern_databases;
}

template <class PDBType>
long long IncrementalPPDBs<PDBType>::get_size() const
{
    return size;
}

template class IncrementalPPDBs<MaxProbProjection>;
template class IncrementalPPDBs<ExpCostProjection>;

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd