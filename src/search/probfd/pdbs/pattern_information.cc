#include "probfd/pdbs/pattern_information.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"

#include "probfd/cost_function.h"

#include <cassert>

using namespace std;

namespace probfd {
namespace pdbs {

PatternInformation::PatternInformation(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction* task_cost_function,
    Pattern pattern)
    : task_proxy(task_proxy)
    , task_cost_function(task_cost_function)
    , pattern(std::move(pattern))
{
}

bool PatternInformation::information_is_valid() const
{
    return !pdb || pdb->get_pattern() == pattern;
}

void PatternInformation::create_pdb_if_missing()
{
    if (!pdb) {
        pdb = make_shared<ProbabilityAwarePatternDatabase>(
            task_proxy,
            *task_cost_function,
            pattern,
            task_proxy.get_initial_state());
    }
}

void PatternInformation::set_pdb(
    const shared_ptr<ProbabilityAwarePatternDatabase>& pdb_)
{
    pdb = pdb_;
    assert(information_is_valid());
}

const Pattern& PatternInformation::get_pattern() const
{
    return pattern;
}

shared_ptr<ProbabilityAwarePatternDatabase> PatternInformation::get_pdb()
{
    create_pdb_if_missing();
    return pdb;
}

} // namespace pdbs
} // namespace probfd