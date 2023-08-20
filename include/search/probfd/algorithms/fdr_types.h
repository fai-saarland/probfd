#ifndef PROBFD_ALGORITHM_INTERFACES_FDR_TYPES_H
#define PROBFD_ALGORITHM_INTERFACES_FDR_TYPES_H

class State;
class OperatorID;

namespace probfd {
namespace algorithms {

// Behavioural interfaces
template <typename>
class OpenList;

template <typename, typename>
class PolicyPicker;

template <typename>
class SuccessorSampler;

template <typename, typename>
class TransitionSorter;

} // namespace algorithms

/// Type alias for OpenLists for MDPs in FDR.
using FDROpenList = algorithms::OpenList<OperatorID>;

/// Type alias for policy pickers for MDPs in FDR.
using FDRPolicyPicker = algorithms::PolicyPicker<State, OperatorID>;

// Type alias for successor samplers for MDPs in FDR.
using FDRSuccessorSampler = algorithms::SuccessorSampler<OperatorID>;

/// Type alias for TransitionSorters for MDPs in FDR.
using FDRTransitionSorter = algorithms::TransitionSorter<State, OperatorID>;

} // namespace probfd

#endif