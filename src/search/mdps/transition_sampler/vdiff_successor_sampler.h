#pragma once

#include "../transition_sampler.h"
#include "../utils/distribution_random_sampler.h"

#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {
namespace transition_sampler {

class VDiffSuccessorSampler : public ProbabilisticOperatorTransitionSampler {
public:
    explicit VDiffSuccessorSampler(const options::Options&);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual StateID sample(
        const StateID& state,
        const ProbabilisticOperator* op,
        const Distribution<StateID>& successors) override;

    Distribution<StateID> biased_;
    distribution_random_sampler::DistributionRandomSampler sampler_;

    const bool prefer_large_gaps_;
};

} // namespace transition_sampler
} // namespace probabilistic
