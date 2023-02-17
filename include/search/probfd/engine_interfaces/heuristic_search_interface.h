#ifndef PROBFD_ENGINE_INTERFACES_HEURISTIC_SEARCH_INTERFACE_H
#define PROBFD_ENGINE_INTERFACES_HEURISTIC_SEARCH_INTERFACE_H

#include "probfd/engines/heuristic_search_state_information.h"

namespace probfd {
namespace engine_interfaces {

class HeuristicSearchInterface {
public:
    virtual ~HeuristicSearchInterface() = default;

    virtual const engines::heuristic_search::StateFlags&
    lookup_state_flags(StateID state_id) = 0;

    virtual value_t lookup_value(StateID state_id) = 0;

    virtual Interval lookup_dual_bounds(StateID state_id) = 0;

    virtual ActionID lookup_policy(StateID state_id) = 0;
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __HEURISTIC_SEARCH_CONNECTOR_H__