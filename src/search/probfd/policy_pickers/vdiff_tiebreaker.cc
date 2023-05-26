#include "probfd/policy_pickers/vdiff_tiebreaker.h"

#include "probfd/engine_interfaces/state_properties.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace policy_pickers {

VDiffTiebreaker::VDiffTiebreaker(const options::Options& opts)
    : VDiffTiebreaker(
          opts.get<bool>("stable_policy"),
          opts.get<bool>("prefer_large_gaps") ? -1 : 1)
{
}

VDiffTiebreaker::VDiffTiebreaker(bool stable_policy, value_t favor_large_gaps)
    : TaskStablePolicyPicker<VDiffTiebreaker>(stable_policy)
    , favor_large_gaps_(favor_large_gaps)
{
}

int VDiffTiebreaker::pick_index(
    engine_interfaces::StateSpace<State, OperatorID>&,
    StateID,
    std::optional<OperatorID>,
    const std::vector<OperatorID>&,
    const std::vector<Distribution<StateID>>& successors,
    engine_interfaces::StateProperties& properties)
{
    value_t best = INFINITE_VALUE;
    unsigned choice = 1;
    for (int i = successors.size() - 1; i >= 0; --i) {
        const Distribution<StateID>& t = successors[i];
        value_t sum = 0_vt;
        for (auto it = t.begin(); it != t.end(); ++it) {
            auto value = properties.lookup_bounds(it->item);
            sum += it->probability * value.length();
        }
        if (is_approx_less(favor_large_gaps_ * sum, best)) {
            best = sum;
            choice = i;
        }
    }
    return choice;
}

void VDiffTiebreaker::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<bool>("stable_policy", "", "true");
    parser.add_option<bool>("prefer_large_gaps", "", "true");
}

} // namespace policy_pickers
} // namespace probfd
