#ifndef PROBFD_CARTESIAN_EVALUATORS_H
#define PROBFD_CARTESIAN_EVALUATORS_H

#include "probfd/evaluator.h"
#include "probfd/value_type.h"

#include <vector>

namespace probfd {
namespace cartesian {

class AbstractState;

class CartesianHeuristic : public Evaluator<int> {
    std::vector<value_t> h_values = {0.0_vt};

public:
    value_t evaluate(int state) const final override;

    value_t get_h_value(int v) const;
    void set_h_value(int v, value_t h);
    void on_split(int v);
};

} // namespace cartesian
} // namespace probfd

#endif // PROBFD_CARTESIAN_EVALUATORS_H
