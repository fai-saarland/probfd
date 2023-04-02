#ifndef PROBFD_SUCCESSOR_SAMPLERS_ARBITRARY_SELECTOR_FACTORY_H
#define PROBFD_SUCCESSOR_SAMPLERS_ARBITRARY_SELECTOR_FACTORY_H

#include "probfd/successor_samplers/task_successor_sampler_factory.h"

namespace probfd {

/// Namespace dedicated to transition sampling.
namespace successor_samplers {

class ArbitrarySuccessorSelectorFactory : public TaskSuccessorSamplerFactory {
public:
    ~ArbitrarySuccessorSelectorFactory() override = default;

    std::shared_ptr<engine_interfaces::SuccessorSampler<OperatorID>>
    create_sampler(engine_interfaces::StateSpace<State, OperatorID>*) override;
};

} // namespace successor_samplers
} // namespace probfd

#endif // __ARBITRARY_SELECTOR_H__