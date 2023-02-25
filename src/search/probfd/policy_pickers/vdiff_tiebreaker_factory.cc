#include "probfd/policy_pickers/vdiff_tiebreaker_factory.h"
#include "probfd/policy_pickers/vdiff_tiebreaker.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace policy_pickers {

VDiffTiebreakerFactory::VDiffTiebreakerFactory(const options::Options& opts)
    : favor_large_gaps_(opts.get<bool>("prefer_large_gaps") ? -1 : 1)
{
}

void VDiffTiebreakerFactory::add_options_to_parser(
    options::OptionParser& parser)
{
    parser.add_option<bool>("prefer_large_gaps", "", "true");
}

std::shared_ptr<engine_interfaces::PolicyPicker<OperatorID>>
VDiffTiebreakerFactory::create_policy_tiebreaker(
    engine_interfaces::StateSpace<State, OperatorID>*)
{
    return std::make_shared<VDiffTiebreaker>(favor_large_gaps_);
}

} // namespace policy_pickers
} // namespace probfd
