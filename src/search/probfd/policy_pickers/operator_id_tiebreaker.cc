#include "probfd/policy_pickers/operator_id_tiebreaker.h"

#include "operator_id.h"

#include "option_parser.h"
#include "plugin.h"

#include <limits>

namespace probfd {
namespace policy_pickers {

OperatorIdTiebreaker::OperatorIdTiebreaker(const options::Options& opts)
    : OperatorIdTiebreaker(
          opts.get<bool>("stable_policy"),
          opts.get<bool>("prefer_smaller") ? 1 : -1)
{
}

OperatorIdTiebreaker::OperatorIdTiebreaker(bool stable_policy, int ascending)
    : TaskStablePolicyPicker<OperatorIdTiebreaker>(stable_policy)
    , ascending_(ascending)
{
}

int OperatorIdTiebreaker::pick_index(
    engine_interfaces::StateSpace<State, OperatorID>&,
    StateID,
    std::optional<OperatorID>,
    const std::vector<OperatorID>& choices,
    const std::vector<Distribution<StateID>>&,
    engine_interfaces::StateProperties&)
{
    int min_id = std::numeric_limits<int>::max();
    unsigned min_idx = -1;
    for (int i = choices.size() - 1; i >= 0; i--) {
        int id = choices[i].get_index() * ascending_;
        if (id < min_id) {
            min_id = id;
            min_idx = i;
        }
    }
    return min_idx;
}

void OperatorIdTiebreaker::add_options_to_parser(options::OptionParser& p)
{
    p.add_option<bool>("stable_policy", "", "true");
    p.add_option<bool>("prefer_smaller", "", "true");
}

} // namespace policy_pickers
} // namespace probfd
