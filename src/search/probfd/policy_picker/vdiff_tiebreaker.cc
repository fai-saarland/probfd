#include "vdiff_tiebreaker.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../engines/heuristic_search_state_information.h"

namespace probfd {
namespace policy_tiebreaking {

VDiffTiebreaker::VDiffTiebreaker(const options::Options& opts)
    : favor_large_gaps_(opts.get<bool>("prefer_large_gaps") ? -1 : 1)
{
}

void VDiffTiebreaker::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<bool>("prefer_large_gaps", "", "true");
}

int VDiffTiebreaker::pick(
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
            sum += it->second * lookup_dual_bounds(it->first)->error_bound();
        }
        if (value_type::approx_less()(favor_large_gaps_ * sum, best)) {
            best = sum;
            choice = i;
        }
    }
    return choice;
}

static Plugin<ProbabilisticOperatorPolicyPicker> _plugin(
    "value_gap_policy_tiebreaker",
    options::parse<ProbabilisticOperatorPolicyPicker, VDiffTiebreaker>);

} // namespace policy_tiebreaking
} // namespace probfd
