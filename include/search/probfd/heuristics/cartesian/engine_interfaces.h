#ifndef PROBFD_HEURISTICS_CARTESIAN_ENGINE_INTERFACES_H
#define PROBFD_HEURISTICS_CARTESIAN_ENGINE_INTERFACES_H

#include "probfd/heuristics/cartesian/types.h"

#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/evaluator.h"

namespace probfd {
namespace heuristics {
namespace cartesian {

class Abstraction;
struct ProbabilisticTransition;

class CartesianCostFunction
    : public engine_interfaces::StateIndependentCostFunction<
          const AbstractState*,
          const ProbabilisticTransition*> {
    Abstraction& abstraction;
    std::vector<value_t> operator_costs;

public:
    CartesianCostFunction(
        Abstraction& abstraction,
        std::vector<value_t> operator_costs);

    /**
     * @brief Get the termination cost info of the input state.
     */
    TerminationInfo get_termination_info(const AbstractState* s) override;

    /**
     * @brief Get the action cost of the action when applied in a state.
     */
    value_t get_action_cost(const ProbabilisticTransition* t) override;

    /**
     * @brief Get the action cost of the action when applied in a state.
     */
    value_t get_cost(int op_index) const;
};

class CartesianHeuristic
    : public engine_interfaces::Evaluator<const AbstractState*> {
    std::vector<value_t> h_values = {0.0_vt};

public:
    EvaluationResult evaluate(const AbstractState* state) const override;

    value_t get_h_value(int v) const;
    void set_h_value(int v, value_t h);
    void on_split(int v);
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif