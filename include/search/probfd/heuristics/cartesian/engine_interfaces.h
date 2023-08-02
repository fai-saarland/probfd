#ifndef PROBFD_HEURISTICS_CARTESIAN_ENGINE_INTERFACES_H
#define PROBFD_HEURISTICS_CARTESIAN_ENGINE_INTERFACES_H

#include "probfd/engine_interfaces/evaluator.h"

#include "probfd/value_type.h"

#include <vector>

namespace probfd {
namespace heuristics {
namespace cartesian {

class AbstractState;

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