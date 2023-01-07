#include "probfd/policy_picker/operator_id_tiebreaker_factory.h"
#include "probfd/policy_picker/operator_id_tiebreaker.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace policy_tiebreaking {

OperatorIdTiebreakerFactory::OperatorIdTiebreakerFactory(
    const options::Options& opts)
    : ascending_(opts.get<bool>("prefer_smaller") ? 1 : -1)
{
}

void OperatorIdTiebreakerFactory::add_options_to_parser(
    options::OptionParser& p)
{
    p.add_option<bool>("prefer_smaller", "", "true");
}

std::shared_ptr<engine_interfaces::PolicyPicker<OperatorID>>
OperatorIdTiebreakerFactory::create_policy_tiebreaker(
    engine_interfaces::StateIDMap<State>*,
    engine_interfaces::ActionIDMap<OperatorID>*)
{
    return std::make_shared<OperatorIdTiebreaker>(ascending_);
}

} // namespace policy_tiebreaking
} // namespace probfd
