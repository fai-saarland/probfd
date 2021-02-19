#pragma once

#include "../engines/heuristic_search_state_information.h"

namespace probabilistic {

class HeuristicSearchConnector {
public:
    void
    set_lookup_function(heuristic_search::PerStateInformationLookup* lookup)
    {
        lookup_ = lookup;
    }

    const void* lookup(const StateID& state_id) const
    {
        return lookup_->lookup(state_id);
    }

private:
    heuristic_search::PerStateInformationLookup* lookup_ = nullptr;
};

} // namespace probabilistic
