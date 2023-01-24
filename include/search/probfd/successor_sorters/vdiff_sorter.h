#ifndef PROBFD_SUCCESSOR_SORTERS_VDIFF_SORTER_H
#define PROBFD_SUCCESSOR_SORTERS_VDIFF_SORTER_H

#include "probfd/engine_interfaces/successor_sorter.h"

namespace probfd {
namespace successor_sorters {

class VDiffSorter : public TaskSuccessorSorter {
    const value_t favor_large_gaps_;

public:
    explicit VDiffSorter(const value_t favor_large_gaps);

protected:
    virtual void sort(
        const StateID& state,
        const std::vector<OperatorID>& action_choices,
        std::vector<Distribution<StateID>>& successors,
        engine_interfaces::HeuristicSearchInterface& hs_interface) override;
};

} // namespace successor_sorters
} // namespace probfd

#endif // __VDIFF_TIEBREAKER_H__