#include "probfd/transition_sampler/most_likely_selector_factory.h"
#include "probfd/transition_sampler/most_likely_selector.h"

namespace probfd {
namespace transition_sampler {

std::shared_ptr<engine_interfaces::TransitionSampler<OperatorID>>
MostLikelySuccessorSelectorFactory::create_sampler(
    engine_interfaces::HeuristicSearchConnector*,
    engine_interfaces::StateIDMap<State>*,
    engine_interfaces::ActionIDMap<OperatorID>*)
{
    return std::shared_ptr<MostLikelySuccessorSelector>();
}

} // namespace transition_sampler
} // namespace probfd
