#include "probfd/transition_sampler/arbitrary_selector_factory.h"
#include "probfd/transition_sampler/arbitrary_selector.h"

namespace probfd {
namespace transition_sampler {

std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>
ArbitrarySuccessorSelectorFactory::create_sampler(
    engine_interfaces::HeuristicSearchConnector*,
    engine_interfaces::StateIDMap<State>*,
    engine_interfaces::ActionIDMap<OperatorID>*)
{
    return std::shared_ptr<ArbitrarySuccessorSelector>();
}

} // namespace transition_sampler
} // namespace probfd
