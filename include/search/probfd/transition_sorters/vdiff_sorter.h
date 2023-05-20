#ifndef PROBFD_TRANSITION_SORTERS_VDIFF_SORTER_H
#define PROBFD_TRANSITION_SORTERS_VDIFF_SORTER_H

#include "probfd/engine_interfaces/transition_sorter.h"

namespace probfd {
namespace transition_sorters {

class VDiffSorter : public TaskTransitionSorter {
    const value_t favor_large_gaps_;

public:
    explicit VDiffSorter(const value_t favor_large_gaps);

protected:
    virtual void sort(
        StateID state,
        const std::vector<OperatorID>& action_choices,
        std::vector<Distribution<StateID>>& successors,
        engine_interfaces::HeuristicSearchInterface<OperatorID>& hs_interface)
        override;
};

} // namespace transition_sorters
} // namespace probfd

#endif // __VDIFF_TIEBREAKER_H__