#ifndef PROBFD_SUCCESSOR_SAMPLERS_VDIFF_SUCCESSOR_SAMPLER_FACTORY_H
#define PROBFD_SUCCESSOR_SAMPLERS_VDIFF_SUCCESSOR_SAMPLER_FACTORY_H

#include "probfd/successor_samplers/task_successor_sampler_factory.h"

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace successor_samplers {

class VDiffSuccessorSamplerFactory : public TaskSuccessorSamplerFactory {
    std::shared_ptr<utils::RandomNumberGenerator> rng;
    const bool prefer_large_gaps;

public:
    explicit VDiffSuccessorSamplerFactory(const options::Options&);
    ~VDiffSuccessorSamplerFactory() override = default;

    static void add_options_to_parser(options::OptionParser& parser);

    std::shared_ptr<engine_interfaces::SuccessorSampler<OperatorID>>
    create_sampler(engine_interfaces::StateSpace<State, OperatorID>*) override;
};

} // namespace successor_samplers
} // namespace probfd

#endif // __VDIFF_SUCCESSOR_SAMPLER_H__