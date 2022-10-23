#include "probfd/transition_sampler/hbiased_successor_sampler.h"

#include "probfd/new_state_handlers/store_heuristic.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace transition_sampler {

HBiasedSuccessorSampler::HBiasedSuccessorSampler(const options::Options& opts)
    : heuristic_(std::dynamic_pointer_cast<new_state_handlers::StoreHeuristic>(
          opts.get<std::shared_ptr<NewGlobalStateHandler>>("heuristic")))
{
}

void HBiasedSuccessorSampler::add_options_to_parser(options::OptionParser& p)
{
    p.add_option<std::shared_ptr<NewGlobalStateHandler>>("heuristic");
}

StateID HBiasedSuccessorSampler::sample(
    const StateID&,
    const ProbabilisticOperator*,
    const Distribution<StateID>& successors)
{
    biased_.clear();
    value_type::value_t sum = 0;
    for (auto it = successors.begin(); it != successors.end(); ++it) {
        const int h = heuristic_->get_cached_h_value(it->element);
        if (h >= 0) {
            const value_type::value_t p =
                it->probability / value_type::value_t(h + 1);
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

static Plugin<ProbabilisticOperatorTransitionSampler> _plugin(
    "hbiased_successor_sampler",
    options::
        parse<ProbabilisticOperatorTransitionSampler, HBiasedSuccessorSampler>);

} // namespace transition_sampler
} // namespace probfd
