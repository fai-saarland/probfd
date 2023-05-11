#ifndef PROBFD_HEURISTICS_CARTESIAN_ENGINE_INTERFACES_H
#define PROBFD_HEURISTICS_CARTESIAN_ENGINE_INTERFACES_H

#include "probfd/heuristics/cartesian/abstract_state.h"
#include "probfd/heuristics/cartesian/abstraction.h"

#include "probfd/engine_interfaces/cost_function.h"
#include "probfd/engine_interfaces/evaluator.h"

#include "probfd/heuristics/cartesian/probabilistic_transition.h"

namespace probfd {
namespace heuristics {
namespace cartesian {

class CartesianCostFunction
    : public engine_interfaces::StateIndependentCostFunction<
          const AbstractState*,
          const ProbabilisticTransition*> {
    Abstraction& abstraction;
    std::vector<value_t> operator_costs;

public:
    CartesianCostFunction(
        Abstraction& abstraction,
        std::vector<value_t> operator_costs)
        : abstraction(abstraction)
        , operator_costs(std::move(operator_costs))
    {
    }

    /**
     * @brief Get the termination cost info of the input state.
     */
    TerminationInfo get_termination_info(const AbstractState* s) override
    {
        if (abstraction.get_goals().contains(s->get_id())) {
            return TerminationInfo(true, 0.0_vt);
        }

        return TerminationInfo(false, INFINITE_VALUE);
    }

    /**
     * @brief Get the action cost of the action when applied in a state.
     */
    value_t get_action_cost(const ProbabilisticTransition* t) override
    {
        return operator_costs[t->op_id];
    }

    /**
     * @brief Get the action cost of the action when applied in a state.
     */
    value_t get_cost(int op_index) const { return operator_costs[op_index]; }
};

class CartesianHeuristic
    : public engine_interfaces::Evaluator<const AbstractState*> {
    std::vector<value_t> h_values = {0.0_vt};

public:
    EvaluationResult evaluate(const AbstractState* state) const override
    {
        assert(utils::in_bounds(state->get_id(), h_values));
        const value_t h = h_values[state->get_id()];
        return EvaluationResult(h == INFINITE_VALUE, h);
    }

    value_t get_h_value(int v) const { return h_values[v]; }
    void set_h_value(int v, value_t h) { h_values[v] = h; }
    void on_split(int v) { h_values.push_back(h_values[v]); }
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif