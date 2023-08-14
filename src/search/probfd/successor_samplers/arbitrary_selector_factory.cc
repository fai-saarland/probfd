#include "probfd/successor_samplers/arbitrary_selector_factory.h"
#include "probfd/successor_samplers/arbitrary_selector.h"

namespace probfd {
namespace successor_samplers {

std::shared_ptr<FDRSuccessorSampler>
ArbitrarySuccessorSelectorFactory::create_sampler(FDRMDP*)
{
    return std::make_shared<ArbitrarySuccessorSelector>();
}

} // namespace successor_samplers
} // namespace probfd
