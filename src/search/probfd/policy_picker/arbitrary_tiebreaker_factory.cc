#include "probfd/policy_picker/arbitrary_tiebreaker_factory.h"
#include "probfd/policy_picker/arbitrary_tiebreaker.h"

namespace probfd {
namespace policy_tiebreaking {

std::shared_ptr<engine_interfaces::PolicyPicker<OperatorID>>
ArbitraryTiebreakerFactory::create_policy_tiebreaker(
    engine_interfaces::HeuristicSearchConnector*,
    engine_interfaces::StateIDMap<State>*,
    engine_interfaces::ActionIDMap<OperatorID>*)
{
    return std::make_shared<ArbitraryTiebreaker<OperatorID>>();
}

} // namespace policy_tiebreaking
} // namespace probfd
