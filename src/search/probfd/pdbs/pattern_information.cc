#include "probfd/pdbs/pattern_information.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"

#include <cassert>
#include <utility>
#include <vector>

using namespace std;

namespace probfd::pdbs {

PatternInformation::PatternInformation(
    ProbabilisticTaskProxy task_proxy,
    FDRSimpleCostFunction* task_cost_function,
    Pattern pattern)
    : task_proxy_(task_proxy)
    , task_cost_function_(task_cost_function)
    , pattern_(std::move(pattern))
{
}

bool PatternInformation::information_is_valid() const
{
    return !pdb_ || pdb_->get_pattern() == pattern_;
}

void PatternInformation::create_pdb_if_missing()
{
    if (!pdb_) {
        pdb_ = make_shared<ProbabilityAwarePatternDatabase>(
            task_proxy_,
            *task_cost_function_,
            pattern_,
            task_proxy_.get_initial_state());
    }
}

void PatternInformation::set_pdb(
    const shared_ptr<ProbabilityAwarePatternDatabase>& pdb)
{
    pdb_ = pdb;
    assert(information_is_valid());
}

const Pattern& PatternInformation::get_pattern() const
{
    return pattern_;
}

shared_ptr<ProbabilityAwarePatternDatabase> PatternInformation::get_pdb()
{
    create_pdb_if_missing();
    return pdb_;
}

} // namespace probfd::pdbs
