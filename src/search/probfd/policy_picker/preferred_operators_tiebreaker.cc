#include "probfd/policy_picker/preferred_operators_tiebreaker.h"

#include "probfd/new_state_handlers/store_preferred_operators.h"

#include "option_parser.h"
#include "plugin.h"

#include <algorithm>

namespace probfd {
namespace policy_tiebreaking {

PreferredOperatorsTiebreaker::PreferredOperatorsTiebreaker(
    const options::Options& opts)
    : pref_ops_(std::dynamic_pointer_cast<
                new_state_handlers::StorePreferredOperators>(
          opts.get<std::shared_ptr<NewGlobalStateHandler>>("heuristic")))
{
}

void PreferredOperatorsTiebreaker::add_options_to_parser(
    options::OptionParser& p)
{
    p.add_option<std::shared_ptr<NewGlobalStateHandler>>("heuristic");
}

int PreferredOperatorsTiebreaker::pick(
    const StateID& state,
    const ActionID&,
    const std::vector<const ProbabilisticOperator*>& choices,
    const std::vector<Distribution<StateID>>&)
{
    const new_state_handlers::PrefOpsCacheEntry& e =
        pref_ops_->get_cached_ops(state);
    unsigned j = -1;
    for (unsigned i = 0; i < choices.size(); ++i) {
        if (e.contains(choices[i])) {
            j = i;
            break;
        }
    }
    if (j >= choices.size()) {
        j = 0;
    }
    return j;
}

static Plugin<ProbabilisticOperatorPolicyPicker> _plugin(
    "preferred_operators_policy_tiebreaker",
    options::
        parse<ProbabilisticOperatorPolicyPicker, PreferredOperatorsTiebreaker>);

} // namespace policy_tiebreaking
} // namespace probfd
