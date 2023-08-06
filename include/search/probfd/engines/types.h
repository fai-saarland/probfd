#ifndef PROBFD_ENGINES_TYPES_H
#define PROBFD_ENGINES_TYPES_H

#include "probfd/interval.h"

#include <type_traits>

namespace probfd {
namespace engines {

/// Convenience value type alias for engines selecting interval iteration
/// behaviour based on a template parameter.
template <bool UseInterval>
using EngineValueType = std::conditional_t<UseInterval, Interval, value_t>;

} // namespace engines
} // namespace probfd

#endif