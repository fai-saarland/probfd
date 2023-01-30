#ifndef MDPS_TRANSITION_SAMPLER_VDIFF_SUCCESSOR_SAMPLER_H
#define MDPS_TRANSITION_SAMPLER_VDIFF_SUCCESSOR_SAMPLER_H

#include "probfd/transition_sampler.h"

#include "probfd/utils/distribution_random_sampler.h"

#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace utils {
class RandomNumberGenerator;
}

namespace probfd {
namespace transition_sampler {

class VDiffSuccessorSampler : public ProbabilisticOperatorTransitionSampler {
public:
    explicit VDiffSuccessorSampler(const options::Options&);
    explicit VDiffSuccessorSampler(
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        bool prefer_large_gaps);

    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual StateID sample(
        const StateID& state,
        OperatorID op,
        const Distribution<StateID>& successors) override;

    Distribution<StateID> biased_;
    distribution_random_sampler::DistributionRandomSampler sampler_;

    const bool prefer_large_gaps_;
};

} // namespace transition_sampler
} // namespace probfd

#endif // __VDIFF_SUCCESSOR_SAMPLER_H__