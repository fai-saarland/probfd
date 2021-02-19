#pragma once

#include "../transition_sampler.h"
#include "../utils/distribution_random_sampler.h"

#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {

namespace new_state_handlers {
class StoreHeuristic;
}

namespace transition_sampler {

class HBiasedSuccessorSampler : public ProbabilisticOperatorTransitionSampler {
public:
    explicit HBiasedSuccessorSampler(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual StateID sample(
        const StateID& state,
        const ProbabilisticOperator* op,
        const Distribution<StateID>& successors) override;

    std::shared_ptr<new_state_handlers::StoreHeuristic> heuristic_;
    Distribution<StateID> biased_;
    distribution_random_sampler::DistributionRandomSampler sampler_;
};

} // namespace transition_sampler
} // namespace probabilistic

