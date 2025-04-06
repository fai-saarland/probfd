#ifndef PROBFD_FDR_TYPES_H
#define PROBFD_FDR_TYPES_H

namespace downward {
class State;
class OperatorID;
} // namespace downward

namespace probfd {

// MDP interface
template <typename, typename>
class StateSpace;

template <typename, typename>
class CostFunction;

template <typename, typename>
class SimpleCostFunction;

template <typename, typename>
class MDP;

template <typename, typename>
class SimpleMDP;

template <typename, typename>
class MDPAlgorithm;

// Heuristics / Evaluators
template <typename>
class Heuristic;

/// Type alias for state spaces in FDR.
using FDRStateSpace = StateSpace<downward::State, downward::OperatorID>;

/// Type alias for cost functions for MDPs in FDR.
using FDRCostFunction =
    SimpleCostFunction<downward::State, downward::OperatorID>;

/// Type alias for simple cost functions for MDPs in FDR.
using FDRSimpleCostFunction =
    SimpleCostFunction<downward::State, downward::OperatorID>;

/// Type alias for MDPs with states in FDR.
using FDRMDP = MDP<downward::State, downward::OperatorID>;

/// Type alias for simple MDPs with states in FDR.
using FDRSimpleMDP = SimpleMDP<downward::State, downward::OperatorID>;

/// Type alias for evaluators for states in FDR.
using FDREvaluator = Heuristic<downward::State>;

// Type alias for search algorithms for MDPs in FDR.
using FDRMDPAlgorithm = MDPAlgorithm<downward::State, downward::OperatorID>;

} // namespace probfd

#endif