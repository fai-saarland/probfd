#ifndef PROBFD_SUCCESSOR_SAMPLERS_TASK_SUCCESSOR_SAMPLER_FACTORY_H
#define PROBFD_SUCCESSOR_SAMPLERS_TASK_SUCCESSOR_SAMPLER_FACTORY_H

#include "probfd/algorithms/fdr_types.h"

#include "probfd/fdr_types.h"

#include <memory>

namespace probfd {

/// Factory interface for transition samplers.
class FDRSuccessorSamplerFactory {
public:
    virtual ~FDRSuccessorSamplerFactory() = default;

    /// Creates a transition sampler from a given state and action id map.
    virtual std::shared_ptr<FDRSuccessorSampler> create_sampler(FDRMDP*) = 0;
};

} // namespace probfd

#endif