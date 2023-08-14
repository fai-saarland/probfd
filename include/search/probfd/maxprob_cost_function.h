#ifndef PROBFD_MAXPROB_COST_FUNCTION_H
#define PROBFD_MAXPROB_COST_FUNCTION_H

#include "probfd/cost_function.h"
#include "probfd/fdr_types.h"
#include "probfd/task_proxy.h"
#include "probfd/value_type.h"

namespace probfd {

class MaxProbCostFunction : public FDRCostFunction {
    ProbabilisticTaskProxy task_proxy;

public:
    explicit MaxProbCostFunction(const ProbabilisticTaskProxy& task_proxy);

    bool is_goal(param_type<State> state) const override;
    value_t get_non_goal_termination_cost() const override final;
    value_t get_action_cost(OperatorID) override;
};

} // namespace probfd

#endif // MAXPROB_COST_FUNCTION_H