#ifndef MDPS_HEURISTICS_PDBS_SUBCOLLECTIONS_INDEPENDENCE_H
#define MDPS_HEURISTICS_PDBS_SUBCOLLECTIONS_INDEPENDENCE_H

#include "../types.h"

namespace probabilistic {
namespace pdbs {

bool is_independent_collection(const PatternCollection& patterns);

} // namespace pdbs
} // namespace probabilistic

#endif // __INDEPENDENCE_H__