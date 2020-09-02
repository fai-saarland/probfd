#include "hbased_successor_sampler.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../new_state_handlers/store_heuristic.h"

namespace probabilistic {
namespace successor_sampler {

HBasedSuccessorSampler::HBasedSuccessorSampler(const options::Options& opts)
    : heuristic_(std::dynamic_pointer_cast<new_state_handlers::StoreHeuristic>(
        opts.get<std::shared_ptr<NewGlobalStateHandler>>("heuristic")))
{
}

void
HBasedSuccessorSampler::add_options_to_parser(options::OptionParser& p)
{
    p.add_option<std::shared_ptr<NewGlobalStateHandler>>("heuristic");
}

GlobalState
HBasedSuccessorSampler::sample(
    const GlobalState&,
    const ProbabilisticOperator*,
    const Distribution<GlobalState>& successors)
{
    biased_.clear();
    value_type::value_t sum = 0;
    for (auto it = successors.begin(); it != successors.end(); ++it) {
        const int h = heuristic_->get_cached_h_value(it->first);
        if (h >= 0) {
            const value_type::value_t p =
                it->second / value_type::value_t(h + 1);
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
    "hbased_sampler",
    options::parse<TransitionSampler, HBasedSuccessorSampler>);

} // namespace successor_sampler
} // namespace probabilistic
