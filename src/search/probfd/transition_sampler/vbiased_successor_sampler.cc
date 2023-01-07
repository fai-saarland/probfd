#include "probfd/transition_sampler/vbiased_successor_sampler.h"

#include "probfd/engine_interfaces/heuristic_search_interface.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "operator_id.h"
#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace transition_sampler {

VBiasedSuccessorSampler::VBiasedSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : sampler_(rng)
{
}

StateID VBiasedSuccessorSampler::sample(
    const StateID&,
    const OperatorID&,
    const Distribution<StateID>& successors,
    engine_interfaces::HeuristicSearchInterface& hs_interface)
{
    biased_.clear();
    value_type::value_t sum = 0;
    for (auto it = successors.begin(); it != successors.end(); ++it) {
        const auto p = it->probability * hs_interface.lookup_value(it->element);
        if (p > value_type::zero) {
            sum += p;
            biased_.add(it->element, p);
        }
    }
    if (biased_.empty()) {
        return sampler_(successors);
    }
    biased_.normalize(value_type::one / sum);
    return sampler_(biased_);
}

} // namespace transition_sampler
} // namespace probfd
