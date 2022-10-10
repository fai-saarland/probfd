#ifndef MDPS_HEURISTICS_PDBS_SUBCOLLECTIONS_INDEPENDENCE_H
#define MDPS_HEURISTICS_PDBS_SUBCOLLECTIONS_INDEPENDENCE_H

#include "../types.h"

namespace probfd {
namespace heuristics {
namespace pdbs {

bool is_independent_collection(const PatternCollection& patterns);

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // __INDEPENDENCE_H__