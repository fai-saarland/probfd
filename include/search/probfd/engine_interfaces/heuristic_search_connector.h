#ifndef MDPS_ENGINE_INTERFACES_HEURISTIC_SEARCH_CONNECTOR_H
#define MDPS_ENGINE_INTERFACES_HEURISTIC_SEARCH_CONNECTOR_H

#include "probfd/engines/heuristic_search_state_information.h"

namespace probfd {
namespace engine_interfaces {

/**
 * \todo Serves as a bridge between the global heuristic search engine and the
 * global engine interfaces constructed by the option parser. But do we really
 * need this? Wouldn't it be much easier to let the heuristic search engine
 * pass itself (or an abstraction which it implements) to the interfaces?
 */
class HeuristicSearchConnector {
public:
    void set_lookup_function(
        engines::heuristic_search::PerStateInformationLookup* lookup)
    {
        lookup_ = lookup;
    }

    const void* lookup(const StateID& state_id) const
    {
        return lookup_->lookup(state_id);
    }

    const engines::heuristic_search::StateFlags*
    lookup_state_flags(const StateID& state_id) const
    {
        return reinterpret_cast<const engines::heuristic_search::StateFlags*>(
            lookup(state_id));
    }

    const value_type::value_t& lookup_value(const StateID& state_id) const
    {
        return *reinterpret_cast<const value_type::value_t*>(lookup(state_id));
    }

    const value_utils::IntervalValue*
    lookup_dual_bounds(const StateID& state_id) const
    {
        return reinterpret_cast<const value_utils::IntervalValue*>(
            lookup(state_id));
    }

    ActionID lookup_policy(const StateID& state_id) const
    {
        return reinterpret_cast<const engines::heuristic_search::StatesPolicy<
            std::true_type>*>(lookup(state_id))
            ->get_policy();
    }

private:
    engines::heuristic_search::PerStateInformationLookup* lookup_ = nullptr;
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __HEURISTIC_SEARCH_CONNECTOR_H__