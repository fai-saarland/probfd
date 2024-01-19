#ifndef PROBFD_MAXPROB_COST_FUNCTION_H
#define PROBFD_MAXPROB_COST_FUNCTION_H

#include "probfd/cost_function.h"
#include "probfd/fdr_types.h"
#include "probfd/task_proxy.h"
#include "probfd/type_traits.h"
#include "probfd/value_type.h"

// Forward Declarations
class OperatorID;
class State;

namespace probfd {

class MaxProbCostFunction : public FDRCostFunction {
    ProbabilisticTaskProxy task_proxy_;

public:
    explicit MaxProbCostFunction(const ProbabilisticTaskProxy& task_proxy);

    [[nodiscard]]
    bool is_goal(param_type<State> state) const override;

    [[nodiscard]]
    value_t get_non_goal_termination_cost() const final;

    value_t get_action_cost(OperatorID) override;
};

} // namespace probfd

#endif // PROBFD_MAXPROB_COST_FUNCTION_H