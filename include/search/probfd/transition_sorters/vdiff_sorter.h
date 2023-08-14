#ifndef PROBFD_TRANSITION_SORTERS_VDIFF_SORTER_H
#define PROBFD_TRANSITION_SORTERS_VDIFF_SORTER_H

#include "probfd/engine_interfaces/fdr_types.h"
#include "probfd/engine_interfaces/transition_sorter.h"

#include "probfd/value_type.h"

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
        engine_interfaces::StateProperties& properties) override;
};

} // namespace transition_sorters
} // namespace probfd

#endif // __VDIFF_TIEBREAKER_H__