#ifndef PROBFD_SUCCESSOR_SAMPLERS_TASK_SUCCESSOR_SAMPLER_FACTORY_H
#define PROBFD_SUCCESSOR_SAMPLERS_TASK_SUCCESSOR_SAMPLER_FACTORY_H

#include "probfd/engine_interfaces/types.h"

#include <memory>

namespace probfd {

/// Factory interface for transition samplers.
class TaskSuccessorSamplerFactory {
public:
    virtual ~TaskSuccessorSamplerFactory() = default;

    /// Creates a transition sampler from a given state and action id map.
    virtual std::shared_ptr<TaskSuccessorSampler> create_sampler(TaskMDP*) = 0;
};

} // namespace probfd

#endif