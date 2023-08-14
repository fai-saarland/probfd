#ifndef PROBFD_SUCCESSOR_SAMPLERS_MOST_LIKELY_SELECTOR_FACTORY_H
#define PROBFD_SUCCESSOR_SAMPLERS_MOST_LIKELY_SELECTOR_FACTORY_H

#include "probfd/successor_samplers/task_successor_sampler_factory.h"

namespace probfd {
namespace successor_samplers {

class MostLikelySuccessorSelectorFactory : public FDRSuccessorSamplerFactory {
public:
    ~MostLikelySuccessorSelectorFactory() override = default;

    std::shared_ptr<FDRSuccessorSampler> create_sampler(FDRMDP*) override;
};

} // namespace successor_samplers
} // namespace probfd

#endif // __MOST_LIKELY_SELECTOR_H__