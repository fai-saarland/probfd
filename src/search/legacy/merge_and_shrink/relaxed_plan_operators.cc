#include "legacy/merge_and_shrink/relaxed_plan_operators.h"

#include "legacy/merge_and_shrink/additive_heuristic.h"

#include "legacy/global_operator.h"
#include "legacy/global_state.h"

#include "option_parser.h"

#include <cassert>
#include <vector>

using namespace std;

namespace legacy {
namespace merge_and_shrink {

// construction and destruction
RelaxedPlanOperators::RelaxedPlanOperators(const options::Options& opts)
    : FFHeuristic(opts)
{
}

RelaxedPlanOperators::~RelaxedPlanOperators()
{
}

void RelaxedPlanOperators::get_relaxed_plan_operators(vector<bool>& labels)
{
    initialize();

    compute_relaxed_plan(g_initial_state(), labels);
}

} // namespace merge_and_shrink
} // namespace legacy