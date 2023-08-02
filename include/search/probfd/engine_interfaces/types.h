#ifndef PROBFD_ENGINE_INTERFACES_TYPES_H
#define PROBFD_ENGINE_INTERFACES_TYPES_H

class State;
class OperatorID;

namespace probfd {
namespace engine_interfaces {

class HeuristicSearchConnector;

// MDP specification
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

// Heuristics / Evaluators
template <typename>
class Evaluator;

// Behavioural interfaces
template <typename>
class NewStateObserver;

template <typename>
class NewStateObserverList;

template <typename>
class OpenList;

template <typename, typename>
class PolicyPicker;

template <typename>
class SuccessorSampler;

template <typename>
class TransitionSorter;

} // namespace engine_interfaces

/// Type alias for cost functions of probabilistic planning tasks.
using TaskStateSpace = engine_interfaces::StateSpace<State, OperatorID>;

/// Type alias for cost functions of probabilistic planning tasks.
using TaskCostFunction =
    engine_interfaces::SimpleCostFunction<State, OperatorID>;

/// Type alias for simple cost functions of probabilistic planning tasks.
using TaskSimpleCostFunction =
    engine_interfaces::SimpleCostFunction<State, OperatorID>;

/// Type alias for MDPs of probabilistic planning tasks.
using TaskMDP = engine_interfaces::MDP<State, OperatorID>;

/// Type alias for simple MDPs of probabilistic planning tasks.
using TaskSimpleMDP = engine_interfaces::SimpleMDP<State, OperatorID>;

/// Type alias for evaluators of planning tasks.
using TaskEvaluator = engine_interfaces::Evaluator<State>;

/// Type alias for NewStateObservers for probabilistic planning tasks.
using TaskNewStateObserver = engine_interfaces::NewStateObserver<State>;

/// Type alias for NewStateObserverLists for probabilistic planning tasks.
using TaskNewStateObserverList = engine_interfaces::NewStateObserverList<State>;

/// Type alias for OpenLists for probabilistic planning tasks.
using TaskOpenList = engine_interfaces::OpenList<OperatorID>;

/// Type alias for policy pickers for probabilistic planning tasks.
using TaskPolicyPicker = engine_interfaces::PolicyPicker<State, OperatorID>;

// Type alias for successor samplers for probabilistic planning tasks.
using TaskSuccessorSampler = engine_interfaces::SuccessorSampler<OperatorID>;

/// Type alias for TransitionSorters for probabilistic planning tasks.
using TaskTransitionSorter = engine_interfaces::TransitionSorter<OperatorID>;

} // namespace probfd

#endif