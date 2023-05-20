#include "probfd/successor_samplers/vbiased_successor_sampler.h"

#include "probfd/engine_interfaces/heuristic_search_interface.h"

#include "utils/rng.h"
#include "utils/rng_options.h"

#include "operator_id.h"
#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace successor_samplers {

VBiasedSuccessorSampler::VBiasedSuccessorSampler(
    std::shared_ptr<utils::RandomNumberGenerator> rng)
    : rng_(rng)
{
}

StateID VBiasedSuccessorSampler::sample(
    StateID,
    OperatorID,
    const Distribution<StateID>& successors,
    engine_interfaces::HeuristicSearchInterface<OperatorID>& hs_interface)
{
    biased_.clear();

    value_t sum = 0;
    for (const auto& [item, probability] : successors) {
        const auto p = probability * hs_interface.lookup_value(item);
        if (p > 0_vt) {
            sum += p;
            biased_.add_probability(item, p);
        }
    }

    if (biased_.empty()) {
        return successors.sample(*rng_)->item;
    }
    biased_.normalize(1_vt / sum);
    return biased_.sample(*rng_)->item;
}

} // namespace successor_samplers
} // namespace probfd
