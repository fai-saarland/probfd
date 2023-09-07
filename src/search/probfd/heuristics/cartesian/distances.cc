#include "probfd/heuristics/abstractions/distances-impl.h"

#include "probfd/heuristics/cartesian/probabilistic_transition.h"

using namespace std;

namespace probfd::heuristics::abstractions {

template void compute_value_table<const cartesian::ProbabilisticTransition*>(
    AbstractMDP<const cartesian::ProbabilisticTransition*>& mdp,
    AbstractStateIndex initial_state,
    const AbstractionEvaluator& heuristic,
    std::span<value_t> value_table,
    double max_time);

} // namespace probfd::heuristics::abstractions