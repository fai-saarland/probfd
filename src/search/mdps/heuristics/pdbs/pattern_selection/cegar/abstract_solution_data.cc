#include "abstract_solution_data.h"

#include "../../../../../global_state.h"

#include "../../../../../utils/hash.h"
#include "../../../../../utils/rng.h"

#include "../../abstract_state_mapper.h"
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

namespace {
template <typename PDBType>
class MergeEvaluator : public StateEvaluator<AbstractState> {
    const AbstractStateMapper* mapper;
    const PDBType& left;
    const PDBType& right;

public:
    explicit MergeEvaluator(
        const AbstractStateMapper* mapper,
        const PDBType& left,
        const PDBType& right)
        : mapper(mapper)
        , left(left)
        , right(right)
    {
    }

protected:
    EvaluationResult evaluate(const AbstractState& state) const override
    {
        AbstractState lstate = mapper->convert(state, left.get_pattern());

        auto leval = left.evaluate(lstate);

        if (leval) {
            return leval;
        }

        AbstractState rstate = mapper->convert(state, right.get_pattern());

        auto reval = right.evaluate(rstate);

        if (reval) {
            return reval;
        }

        return {
            false,
            std::min(
                static_cast<value_type::value_t>(leval),
                static_cast<value_type::value_t>(reval))};
    }
};
} // namespace

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
AbstractSolutionData<PDBType>::AbstractSolutionData(
    const PDBType& previous,
    int add_var,
    std::set<int> blacklist)
    : pdb(new PDBType(previous, add_var))
    , blacklist(std::move(blacklist))
    , policy(pdb->get_optimal_abstract_policy())
    , solved(false)
{
}

template <typename PDBType>
PDBType*
construct_merge_pdb(const PDBType& merge_left, const PDBType& merge_right)
{
    const Pattern& left_pattern = merge_left.get_pattern();
    const Pattern& right_pattern = merge_right.get_pattern();

    Pattern merge_pattern;
    merge_pattern.reserve(left_pattern.size() + right_pattern.size());

    std::merge(
        left_pattern.begin(),
        left_pattern.end(),
        right_pattern.begin(),
        right_pattern.end(),
        std::back_inserter(merge_pattern));

    AbstractStateMapper* mapper =
        new AbstractStateMapper(merge_pattern, ::g_variable_domain);

    return new PDBType(
        mapper,
        MergeEvaluator<PDBType>(mapper, merge_left, merge_right));
}

template <typename PDBType>
AbstractSolutionData<PDBType>::AbstractSolutionData(
    const PDBType& merge_left,
    const PDBType& merge_right,
    std::set<int> blacklist)
    : pdb(construct_merge_pdb(merge_left, merge_right))
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

template <typename PDBType>
bool AbstractSolutionData<PDBType>::solution_exists() const
{
    return !static_cast<bool>(pdb->evaluate(g_initial_state()));
}

template class AbstractSolutionData<MaxProbProjection>;
template class AbstractSolutionData<ExpCostProjection>;

} // namespace pattern_selection
} // namespace pdbs
} // namespace probabilistic