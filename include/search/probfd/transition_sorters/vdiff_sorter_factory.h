#ifndef PROBFD_TRANSITION_SORTERS_VDIFF_SORTER_FACTORY_H
#define PROBFD_TRANSITION_SORTERS_VDIFF_SORTER_FACTORY_H

#include "probfd/transition_sorters/task_transition_sorter_factory.h"

#include "probfd/value_type.h"

#include <memory>

// Forward Declarations
namespace plugins {
class Options;
} // namespace plugins

/// This namespace contains implementations of successor sorting algorithms.
namespace probfd::transition_sorters {

class VDiffSorterFactory : public FDRTransitionSorterFactory {
    const value_t favor_large_gaps_;

public:
    explicit VDiffSorterFactory(const plugins::Options&);

    std::shared_ptr<FDRTransitionSorter>
    create_transition_sorter(FDRMDP* state_space) override;
};

} // namespace probfd::transition_sorters

#endif // PROBFD_TRANSITION_SORTERS_VDIFF_SORTER_FACTORY_H