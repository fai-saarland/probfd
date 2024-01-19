#ifndef PROBFD_ALGORITHMS_TYPES_H
#define PROBFD_ALGORITHMS_TYPES_H

#include "probfd/interval.h"

#include <type_traits>

/// This namespace contains implementations of SSP search algorithms.
namespace probfd::algorithms {

/// Convenience value type alias for algorithms selecting interval iteration
/// behaviour based on a template parameter.
template <bool UseInterval>
using AlgorithmValueType = std::conditional_t<UseInterval, Interval, value_t>;

} // namespace probfd::algorithms

#endif