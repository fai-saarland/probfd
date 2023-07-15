#include "probfd/successor_samplers/most_likely_selector_factory.h"
#include "probfd/successor_samplers/most_likely_selector.h"

namespace probfd {
namespace successor_samplers {

std::shared_ptr<TaskSuccessorSampler>
MostLikelySuccessorSelectorFactory::create_sampler(TaskStateSpace*)
{
    return std::make_shared<MostLikelySuccessorSelector>();
}

} // namespace successor_samplers
} // namespace probfd
