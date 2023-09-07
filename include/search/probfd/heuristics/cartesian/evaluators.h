#ifndef PROBFD_HEURISTICS_CARTESIAN_EVALUATORS_H
#define PROBFD_HEURISTICS_CARTESIAN_EVALUATORS_H

#include "probfd/heuristics/cartesian/types.h"

#include "probfd/evaluator.h"
#include "probfd/value_type.h"

#include <vector>

namespace probfd {
namespace heuristics {
namespace cartesian {

class CartesianHeuristic : public AbstractionEvaluator {
    std::vector<value_t> h_values = {0.0_vt};

public:
    value_t evaluate(AbstractStateIndex state) const final override;

    value_t& operator[](AbstractStateIndex v);
    const value_t& operator[](AbstractStateIndex v) const;

    void on_split(AbstractStateIndex v);
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif // PROBFD_HEURISTICS_CARTESIAN_EVALUATORS_H
