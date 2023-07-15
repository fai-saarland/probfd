#ifndef PROBFD_SUCCESSOR_SAMPLERS_VBIASED_SUCCESSOR_SAMPLER_FACTORY_H
#define PROBFD_SUCCESSOR_SAMPLERS_VBIASED_SUCCESSOR_SAMPLER_FACTORY_H

#include "probfd/successor_samplers/task_successor_sampler_factory.h"

namespace plugins {
class Options;
class Feature;
} // namespace plugins

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {

namespace successor_samplers {

class VBiasedSuccessorSamplerFactory : public TaskSuccessorSamplerFactory {
    std::shared_ptr<utils::RandomNumberGenerator> rng;

public:
    explicit VBiasedSuccessorSamplerFactory(const plugins::Options&);
    ~VBiasedSuccessorSamplerFactory() override = default;

    std::shared_ptr<TaskSuccessorSampler>
    create_sampler(TaskStateSpace*) override;
};

} // namespace successor_samplers
} // namespace probfd

#endif // __VBIASED_SUCCESSOR_SAMPLER_H__