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

namespace successor_sampler {

class VDiffSuccessorSampler : public TransitionSampler {
public:
    explicit VDiffSuccessorSampler(const options::Options&);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual GlobalState sample(
        const GlobalState& state,
        const ProbabilisticOperator* op,
        const Distribution<GlobalState>& successors) override;

    std::shared_ptr<new_state_handlers::StoreHeuristic> heuristic_;
    Distribution<GlobalState> biased_;
    distribution_random_sampler::DistributionRandomSampler sampler_;
};

} // namespace successor_sampler
} // namespace probabilistic

