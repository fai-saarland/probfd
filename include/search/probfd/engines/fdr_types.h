#ifndef PROBFD_ENGINE_INTERFACES_FDR_TYPES_H
#define PROBFD_ENGINE_INTERFACES_FDR_TYPES_H

class State;
class OperatorID;

namespace probfd {
namespace engines {

// Behavioural interfaces
template <typename>
class OpenList;

template <typename, typename>
class PolicyPicker;

template <typename>
class SuccessorSampler;

template <typename, typename>
class TransitionSorter;

} // namespace engines

/// Type alias for OpenLists for MDPs in FDR.
using FDROpenList = engines::OpenList<OperatorID>;

/// Type alias for policy pickers for MDPs in FDR.
using FDRPolicyPicker = engines::PolicyPicker<State, OperatorID>;

// Type alias for successor samplers for MDPs in FDR.
using FDRSuccessorSampler = engines::SuccessorSampler<OperatorID>;

/// Type alias for TransitionSorters for MDPs in FDR.
using FDRTransitionSorter = engines::TransitionSorter<State, OperatorID>;

} // namespace probfd

#endif