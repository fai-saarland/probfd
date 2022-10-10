#ifndef MDPS_ENGINE_INTERACES_HEURISTIC_SEARCH_CONNECTOR_H
#define MDPS_ENGINE_INTERACES_HEURISTIC_SEARCH_CONNECTOR_H

#include "../engines/heuristic_search_state_information.h"

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

private:
    engines::heuristic_search::PerStateInformationLookup* lookup_ = nullptr;
};

} // namespace engine_interfaces
} // namespace probfd

#endif // __HEURISTIC_SEARCH_CONNECTOR_H__