#ifndef MDPS_SUCCESSOR_SORTING_VDIFF_SORTER_H
#define MDPS_SUCCESSOR_SORTING_VDIFF_SORTER_H

#include "probfd/engine_interfaces/heuristic_search_connector.h"
#include "probfd/engine_interfaces/successor_sorter.h"

namespace probfd {

/// Namespace dedicated to policy tiebreaker implementations.
namespace successor_sorting {

class VDiffSorter : public TaskSuccessorSorter {
    engine_interfaces::HeuristicSearchConnector* connector_;
    const value_type::value_t favor_large_gaps_;

public:
    VDiffSorter(
        engine_interfaces::HeuristicSearchConnector* connector,
        const value_type::value_t favor_large_gaps);

protected:
    virtual void sort(
        const StateID& state,
        const std::vector<OperatorID>& action_choices,
        std::vector<Distribution<StateID>>& successors) override;
};

} // namespace successor_sorting
} // namespace probfd

#endif // __VDIFF_TIEBREAKER_H__