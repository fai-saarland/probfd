#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_SAMPLING_FLAW_FINDER_FACTORY_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_SAMPLING_FLAW_FINDER_FACTORY_H

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy_factory.h"

namespace options {
class Options;
}

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

template <typename PDBType>
class SamplingFlawFinderFactory : public FlawFindingStrategyFactory<PDBType> {
    unsigned int violation_threshold;

public:
    SamplingFlawFinderFactory(options::Options& parser);
    SamplingFlawFinderFactory(unsigned int violation_threshold);

    ~SamplingFlawFinderFactory() override = default;

    std::unique_ptr<FlawFindingStrategy<PDBType>>
    create_flaw_finder(const ProbabilisticTask* task) override;

    std::string get_name() const override
    {
        return "Sampling Flaw Finder Factory";
    }
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif