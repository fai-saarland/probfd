#include "probfd/heuristics/pdbs/pattern_information.h"

#include "probfd/heuristics/pdbs/probability_aware_pattern_database.h"

#include "probfd/engine_interfaces/cost_function.h"

#include <cassert>

using namespace std;

namespace probfd {
namespace heuristics {
namespace pdbs {

PatternInformation::PatternInformation(
    const ProbabilisticTaskProxy& task_proxy,
    TaskCostFunction* task_cost_function,
    Pattern pattern)
    : task_proxy(task_proxy)
    , pattern(std::move(pattern))
    , task_cost_function(task_cost_function)
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
            pattern,
            *task_cost_function,
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
} // namespace heuristics
} // namespace probfd