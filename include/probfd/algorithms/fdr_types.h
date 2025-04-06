#ifndef PROBFD_ALGORITHMS_FDR_TYPES_H
#define PROBFD_ALGORITHMS_FDR_TYPES_H

namespace downward {
class State;
class OperatorID;
} // namespace downward

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
using FDROpenList = algorithms::OpenList<downward::OperatorID>;

/// Type alias for policy pickers for MDPs in FDR.
using FDRPolicyPicker =
    algorithms::PolicyPicker<downward::State, downward::OperatorID>;

// Type alias for successor samplers for MDPs in FDR.
using FDRSuccessorSampler = algorithms::SuccessorSampler<downward::OperatorID>;

/// Type alias for TransitionSorters for MDPs in FDR.
using FDRTransitionSorter =
    algorithms::TransitionSorter<downward::State, downward::OperatorID>;

} // namespace probfd

#endif