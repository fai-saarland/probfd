#ifndef PROBFD_BISIMULATION_TYPES_H
#define PROBFD_BISIMULATION_TYPES_H

namespace probfd {

/// This namespace contains the implementation of deterministic bisimulation
/// quotients for SSPs, based on merge-and-shrink.
namespace bisimulation {

/// Represents a state in the probabilistic bisimulation quotient.
enum class QuotientState : int {};

/// Represents an action in the probabilistic bisimulation quotient.
enum class QuotientAction : int {};

} // namespace bisimulation
} // namespace probfd

#endif