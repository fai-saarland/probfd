#include "abstract_solution_data.h"

#include "../../../../../global_state.h"

#include "../../../../../utils/hash.h"
#include "../../../../../utils/rng.h"

#include "../../expcost_projection.h"
#include "../../maxprob_projection.h"

#include <functional>
#include <limits>
#include <queue>

using namespace std;
using utils::Verbosity;

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
AbstractSolutionData<PDBType>::AbstractSolutionData(
    const Pattern& pattern,
    set<int> blacklist)
    : pdb(new PDBType(pattern))
    , blacklist(std::move(blacklist))
    , policy(pdb->get_optimal_abstract_policy())
    , solved(false)
{
}

template <typename PDBType>
const Pattern& AbstractSolutionData<PDBType>::get_pattern() const
{
    return pdb->get_pattern();
}

template <typename PDBType>
void AbstractSolutionData<PDBType>::blacklist_variable(int var)
{
    blacklist.insert(var);
}

template <typename PDBType>
bool AbstractSolutionData<PDBType>::is_blacklisted(int var) const
{
    return utils::contains(blacklist, var);
}

template <typename PDBType>
const std::set<int>& AbstractSolutionData<PDBType>::get_blacklist() const
{
    return blacklist;
}

template <typename PDBType>
const PDBType& AbstractSolutionData<PDBType>::get_pdb() const
{
    assert(pdb);
    return *pdb;
}

template <typename PDBType>
std::unique_ptr<PDBType> AbstractSolutionData<PDBType>::steal_pdb()
{
    return std::move(pdb);
}

template <typename PDBType>
const AbstractPolicy& AbstractSolutionData<PDBType>::get_policy() const
{
    return policy;
}

template <typename PDBType>
value_type::value_t AbstractSolutionData<PDBType>::get_policy_cost() const
{
    return pdb->lookup(g_initial_state());
}

template <typename PDBType>
bool AbstractSolutionData<PDBType>::is_solved() const
{
    return solved;
}

template <typename PDBType>
void AbstractSolutionData<PDBType>::mark_as_solved()
{
    solved = true;
}

template class AbstractSolutionData<MaxProbProjection>;
template class AbstractSolutionData<ExpCostProjection>;

} // namespace pattern_selection
} // namespace pdbs
} // namespace probabilistic