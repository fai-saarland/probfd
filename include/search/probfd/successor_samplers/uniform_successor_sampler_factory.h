#ifndef PROBFD_SUCCESSOR_SAMPLERS_UNIFORM_SUCCESSOR_SAMPLER_FACTORY_H
#define PROBFD_SUCCESSOR_SAMPLERS_UNIFORM_SUCCESSOR_SAMPLER_FACTORY_H

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

class UniformSuccessorSamplerFactory : public FDRSuccessorSamplerFactory {
    std::shared_ptr<utils::RandomNumberGenerator> rng;

public:
    explicit UniformSuccessorSamplerFactory(const plugins::Options&);
    ~UniformSuccessorSamplerFactory() override = default;

    std::shared_ptr<FDRSuccessorSampler> create_sampler(FDRMDP*) override;
};

} // namespace successor_samplers
} // namespace probfd

#endif // __UNIFORM_SUCCESSOR_SAMPLER_H__