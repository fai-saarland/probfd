#include "probfd/policy_picker/hbased_tiebreaker.h"

#include "option_parser.h"
#include "plugin.h"
#include "probfd/new_state_handlers/store_heuristic.h"

#include <algorithm>

namespace probfd {
namespace policy_tiebreaking {

HBasedTiebreaker::HBasedTiebreaker(const options::Options& opts)
    : heuristic_(std::dynamic_pointer_cast<new_state_handlers::StoreHeuristic>(
          opts.get<std::shared_ptr<NewGlobalStateHandler>>("heuristic")))
{
}

void HBasedTiebreaker::add_options_to_parser(options::OptionParser& p)
{
    p.add_option<std::shared_ptr<NewGlobalStateHandler>>("heuristic");
}

int HBasedTiebreaker::pick(
    const StateID&,
    const ActionID&,
    const std::vector<const ProbabilisticOperator*>&,
    const std::vector<Distribution<StateID>>& successors)
{
    value_type::value_t best = value_type::inf;
    unsigned choice = 1;
    for (int i = successors.size() - 1; i >= 0; --i) {
        const Distribution<StateID>& t = successors[i];
        value_type::value_t sum = value_type::zero;
        for (auto it = t.begin(); it != t.end(); ++it) {
            const int h = heuristic_->get_cached_h_value(it->element);
            if (h < 0) {
                sum = value_type::inf;
                break;
            }
            sum += it->probability * h;
        }
        if (value_type::approx_less()(sum, best)) {
            best = sum;
            choice = i;
        }
    }
    return choice;
}

static Plugin<ProbabilisticOperatorPolicyPicker> _plugin(
    "hbiased_policy_tiebreaker",
    options::parse<ProbabilisticOperatorPolicyPicker, HBasedTiebreaker>);

} // namespace policy_tiebreaking
} // namespace probfd
