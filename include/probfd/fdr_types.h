#ifndef PROBFD_FDR_TYPES_H
#define PROBFD_FDR_TYPES_H

class State;
class OperatorID;

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
using FDRStateSpace = StateSpace<State, OperatorID>;

/// Type alias for cost functions for MDPs in FDR.
using FDRCostFunction = SimpleCostFunction<State, OperatorID>;

/// Type alias for simple cost functions for MDPs in FDR.
using FDRSimpleCostFunction = SimpleCostFunction<State, OperatorID>;

/// Type alias for MDPs with states in FDR.
using FDRMDP = MDP<State, OperatorID>;

/// Type alias for simple MDPs with states in FDR.
using FDRSimpleMDP = SimpleMDP<State, OperatorID>;

/// Type alias for evaluators for states in FDR.
using FDREvaluator = Heuristic<State>;

// Type alias for search algorithms for MDPs in FDR.
using FDRMDPAlgorithm = MDPAlgorithm<State, OperatorID>;

} // namespace probfd

#endif