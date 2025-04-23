#include "probfd/pdbs/pattern_information.h"

#include "probfd/pdbs/probability_aware_pattern_database.h"

#include "probfd/probabilistic_task.h"

#include <cassert>
#include <utility>
#include <vector>

using namespace std;
using namespace downward;

namespace probfd::pdbs {

PatternInformation::PatternInformation(
    std::shared_ptr<ProbabilisticTask> task,
    Pattern pattern)
    : task_(task)
    , pattern_(std::move(pattern))
    , h(task->get_operators(), *task, *task)

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
            task_->get_variables(),
            pattern_);
        const StateRank istate =
            pdb_->get_abstract_state(task_->get_initial_state());
        compute_distances(*pdb_, task_, istate, h);
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
