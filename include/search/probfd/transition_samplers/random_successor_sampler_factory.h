#ifndef PROBFD_TRANSITION_SAMPLERS_RANDOM_SUCCESSOR_SAMPLER_FACTORY_H
#define PROBFD_TRANSITION_SAMPLERS_RANDOM_SUCCESSOR_SAMPLER_FACTORY_H

#include "probfd/transition_samplers/task_transition_sampler_factory.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace transition_samplers {

class RandomSuccessorSamplerFactory : public TaskTransitionSamplerFactory {
    std::shared_ptr<utils::RandomNumberGenerator> rng;

public:
    explicit RandomSuccessorSamplerFactory(const options::Options&);
    ~RandomSuccessorSamplerFactory() override = default;

    static void add_options_to_parser(options::OptionParser& parser);

    std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>
    create_sampler(
        engine_interfaces::StateIDMap<State>* state_id_map,
        engine_interfaces::ActionIDMap<OperatorID>* op_id_map) override;
};

} // namespace transition_samplers
} // namespace probfd

#endif // __RANDOM_SUCCESSOR_SAMPLER_H__