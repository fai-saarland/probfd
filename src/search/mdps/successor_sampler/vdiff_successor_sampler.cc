#include "vdiff_successor_sampler.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../algorithms/heuristic_search_base.h"

namespace probabilistic {
namespace successor_sampler {

using StateInfo =
    algorithms::heuristic_search_base::HeuristicSearchStateInformation<
        std::true_type>;

VDiffSuccessorSampler::VDiffSuccessorSampler(const options::Options&) { }

void
VDiffSuccessorSampler::add_options_to_parser(options::OptionParser&)
{
}

GlobalState
VDiffSuccessorSampler::sample(
    const GlobalState&,
    const ProbabilisticOperator*,
    const Distribution<GlobalState>& successors)
{
    biased_.clear();
    value_type::value_t sum = 0;
    for (auto it = successors.begin(); it != successors.end(); ++it) {
        const StateInfo* info = static_cast<const StateInfo*>(
            hs_interface_->lookup_state_info(it->first));
        const value_type::value_t p =
            it->second * value_type::abs(info->value - info->value2);
        if (p > value_type::zero) {
            sum += p;
            biased_.add(it->first, p);
        }
    }
    if (biased_.empty()) {
        return sampler_(successors);
    }
    biased_.normalize(value_type::one / sum);
    return sampler_(biased_);
}

static Plugin<TransitionSampler> _plugin(
    "vdiff_sampler",
    options::parse<TransitionSampler, VDiffSuccessorSampler>);

} // namespace successor_sampler
} // namespace probabilistic
