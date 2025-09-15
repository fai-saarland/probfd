#include "probfd/pdbs/pattern_information.h"

#include "downward/initial_state_values.h"
#include "probfd/pdbs/probability_aware_pattern_database.h"

#include "probfd/probabilistic_task.h"

#include <cassert>
#include <utility>
#include <vector>

using namespace std;
using namespace downward;

namespace probfd::pdbs {

PatternInformation::PatternInformation(
    SharedProbabilisticTask task,
    Pattern pattern)
    : task_(std::move(task))
    , pattern_(std::move(pattern))
    , h(get_operators(task_),
        get_cost_function(task_),
        get_termination_costs(task_))

{
}

bool PatternInformation::information_is_valid() const
{
    return !pdb_ || pdb_->get_pattern() == pattern_;
}

void PatternInformation::create_pdb_if_missing()
{
    const auto& variables = get_variables(task_);
    const auto& init_vals = get_init(task_);

    if (!pdb_) {
        pdb_ =
            make_shared<ProbabilityAwarePatternDatabase>(variables, pattern_);
        const StateRank istate =
            pdb_->get_abstract_state(init_vals.get_initial_state());
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
