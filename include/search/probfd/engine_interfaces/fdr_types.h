#ifndef PROBFD_ENGINE_INTERFACES_FDR_TYPES_H
#define PROBFD_ENGINE_INTERFACES_FDR_TYPES_H

class State;
class OperatorID;

namespace probfd {
namespace engine_interfaces {

// Behavioural interfaces
template <typename>
class OpenList;

template <typename, typename>
class PolicyPicker;

template <typename>
class SuccessorSampler;

template <typename, typename>
class TransitionSorter;

} // namespace engine_interfaces

/// Type alias for OpenLists for MDPs in FDR.
using FDROpenList = engine_interfaces::OpenList<OperatorID>;

/// Type alias for policy pickers for MDPs in FDR.
using FDRPolicyPicker = engine_interfaces::PolicyPicker<State, OperatorID>;

// Type alias for successor samplers for MDPs in FDR.
using FDRSuccessorSampler = engine_interfaces::SuccessorSampler<OperatorID>;

/// Type alias for TransitionSorters for MDPs in FDR.
using FDRTransitionSorter =
    engine_interfaces::TransitionSorter<State, OperatorID>;

} // namespace probfd

#endif