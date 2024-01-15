#ifndef PROBFD_TRANSITION_SORTERS_VDIFF_SORTER_H
#define PROBFD_TRANSITION_SORTERS_VDIFF_SORTER_H

#include "probfd/algorithms/fdr_types.h"
#include "probfd/algorithms/transition_sorter.h"

#include "probfd/value_type.h"

#include <vector>

namespace probfd {
namespace transition_sorters {

class VDiffSorter : public FDRTransitionSorter {
    const value_t favor_large_gaps_;

public:
    explicit VDiffSorter(value_t favor_large_gaps);

protected:
    virtual void sort(
        const State& state,
        const std::vector<OperatorID>& action_choices,
        std::vector<Distribution<StateID>>& successors,
        algorithms::StateProperties& properties) override;
};

} // namespace transition_sorters
} // namespace probfd

#endif // __VDIFF_TIEBREAKER_H__