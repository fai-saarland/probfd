#include "probfd/transition_samplers/arbitrary_selector_factory.h"
#include "probfd/transition_samplers/arbitrary_selector.h"

namespace probfd {
namespace transition_samplers {

std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>
ArbitrarySuccessorSelectorFactory::create_sampler(
    engine_interfaces::StateSpace<State, OperatorID>*)
{
    return std::shared_ptr<ArbitrarySuccessorSelector>();
}

} // namespace transition_samplers
} // namespace probfd
