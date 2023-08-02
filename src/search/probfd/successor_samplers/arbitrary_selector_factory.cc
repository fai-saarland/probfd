#include "probfd/successor_samplers/arbitrary_selector_factory.h"
#include "probfd/successor_samplers/arbitrary_selector.h"

namespace probfd {
namespace successor_samplers {

std::shared_ptr<TaskSuccessorSampler>
ArbitrarySuccessorSelectorFactory::create_sampler(TaskMDP*)
{
    return std::make_shared<ArbitrarySuccessorSelector>();
}

} // namespace successor_samplers
} // namespace probfd
