#include "relaxed_plan_operators.h"

#include "additive_heuristic.h"

#include "../global_operator.h"
#include "../option_parser.h"
#include "../global_state.h"

#include <cassert>
#include <vector>
using namespace std;

namespace merge_and_shrink {


// construction and destruction
RelaxedPlanOperators::RelaxedPlanOperators(const options::Options &opts)
    : FFHeuristic(opts)
{
}

RelaxedPlanOperators::~RelaxedPlanOperators()
{
}


void RelaxedPlanOperators::get_relaxed_plan_operators(vector<bool> &labels)
{
    initialize();

    compute_relaxed_plan(g_initial_state(), labels);
}

}
