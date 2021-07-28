#include "random_tiebreaker.h"

#include "../../globals.h"
#include "../../option_parser.h"
#include "../../plugin.h"
#include "../../utils/rng.h"

namespace probabilistic {
namespace policy_tiebreaking {

RandomTiebreaker::RandomTiebreaker(const options::Options&)
{
}

void RandomTiebreaker::add_options_to_parser(options::OptionParser&)
{
}

int RandomTiebreaker::pick(
    const StateID&,
    const ActionID&,
    const std::vector<const ProbabilisticOperator*>& options,
    const std::vector<Distribution<StateID>>&)
{
    return ::g_rng(options.size());
}

static Plugin<ProbabilisticOperatorPolicyPicker> _plugin(
    "random_policy_tiebreaker",
    options::parse<ProbabilisticOperatorPolicyPicker, RandomTiebreaker>);

} // namespace policy_tiebreaking
} // namespace probabilistic
