#include "probfd/policy_pickers/arbitrary_tiebreaker_factory.h"
#include "probfd/policy_pickers/arbitrary_tiebreaker.h"

namespace probfd {
namespace policy_pickers {

std::shared_ptr<engine_interfaces::PolicyPicker<OperatorID>>
ArbitraryTiebreakerFactory::create_policy_tiebreaker(
    engine_interfaces::StateSpace<State, OperatorID>*)
{
    return std::make_shared<ArbitraryTiebreaker<OperatorID>>();
}

} // namespace policy_pickers
} // namespace probfd
