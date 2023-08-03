#ifndef PROBFD_TASK_TYPES_H
#define PROBFD_TASK_TYPES_H

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
class MDPEngine;

// Heuristics / Evaluators
template <typename>
class Evaluator;

/// Type alias for cost functions of probabilistic planning tasks.
using TaskStateSpace = StateSpace<State, OperatorID>;

/// Type alias for cost functions of probabilistic planning tasks.
using TaskCostFunction = SimpleCostFunction<State, OperatorID>;

/// Type alias for simple cost functions of probabilistic planning tasks.
using TaskSimpleCostFunction = SimpleCostFunction<State, OperatorID>;

/// Type alias for MDPs of probabilistic planning tasks.
using TaskMDP = MDP<State, OperatorID>;

/// Type alias for simple MDPs of probabilistic planning tasks.
using TaskSimpleMDP = SimpleMDP<State, OperatorID>;

/// Type alias for evaluators of planning tasks.
using TaskEvaluator = Evaluator<State>;

// Type alias for search engines for planning task MDPs.
using TaskMDPEngine = MDPEngine<State, OperatorID>;

} // namespace probfd

#endif