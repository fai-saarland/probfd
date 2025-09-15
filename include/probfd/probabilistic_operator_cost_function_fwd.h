#ifndef PROBFD_PROBABILISTIC_OPERATOR_COST_FUNCTION_FWD_H
#define PROBFD_PROBABILISTIC_OPERATOR_COST_FUNCTION_FWD_H

#include "probfd/aliases.h"

#include "downward/operator_cost_function_fwd.h"

namespace probfd {
using ProbabilisticOperatorCostFunction =
    downward::OperatorCostFunction<value_t>;
}

#endif
