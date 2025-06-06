#include "downward/operator_cost.h"

#include "downward/task_proxy.h"

#include "downward/utils/system.h"

using namespace std;

namespace downward {

static int
get_adjusted_action_cost(int cost, OperatorCost cost_type, bool is_unit_cost)
{
    switch (cost_type) {
    case NORMAL: return cost;
    case ONE: return 1;
    case PLUSONE:
        if (is_unit_cost)
            return 1;
        else
            return cost + 1;
    default: ABORT("Unknown cost type");
    }
}

int get_adjusted_action_cost(
    const AxiomOrOperatorProxy& op,
    OperatorCost cost_type,
    bool is_unit_cost)
{
    if (op.is_axiom())
        return 0;
    else
        return get_adjusted_action_cost(
            static_cast<OperatorProxy>(op).get_cost(),
            cost_type,
            is_unit_cost);
}

} // namespace downward