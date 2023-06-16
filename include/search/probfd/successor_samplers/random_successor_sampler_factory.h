#ifndef PROBFD_SUCCESSOR_SAMPLERS_RANDOM_SUCCESSOR_SAMPLER_FACTORY_H
#define PROBFD_SUCCESSOR_SAMPLERS_RANDOM_SUCCESSOR_SAMPLER_FACTORY_H

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

class RandomSuccessorSamplerFactory : public TaskSuccessorSamplerFactory {
    std::shared_ptr<utils::RandomNumberGenerator> rng;

public:
    explicit RandomSuccessorSamplerFactory(const plugins::Options&);
    ~RandomSuccessorSamplerFactory() override = default;

    static void add_options_to_feature(plugins::Feature& feature);

    std::shared_ptr<engine_interfaces::SuccessorSampler<OperatorID>>
    create_sampler(engine_interfaces::StateSpace<State, OperatorID>*) override;
};

} // namespace successor_samplers
} // namespace probfd

#endif // __RANDOM_SUCCESSOR_SAMPLER_H__