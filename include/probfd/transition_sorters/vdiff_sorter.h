#ifndef PROBFD_TRANSITION_SORTERS_VDIFF_SORTER_H
#define PROBFD_TRANSITION_SORTERS_VDIFF_SORTER_H

#include "probfd/algorithms/fdr_types.h"
#include "probfd/algorithms/transition_sorter.h"

#include "probfd/value_type.h"

#include <vector>

namespace probfd::transition_sorters {

class VDiffSorter : public FDRTransitionSorter {
    const value_t favor_large_gaps_;

public:
    explicit VDiffSorter(value_t favor_large_gaps);

protected:
    void sort(
        const downward::State& state,
        const std::vector<downward::OperatorID>& action_choices,
        std::vector<SuccessorDistribution>& successor_dist,
        algorithms::StateProperties& properties) override;
};

} // namespace probfd::transition_sorters

#endif // PROBFD_TRANSITION_SORTERS_VDIFF_SORTER_H