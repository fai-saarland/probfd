#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_PUCS_FLAW_FINDER_FACTORY_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_PUCS_FLAW_FINDER_FACTORY_H

#include "probfd/heuristics/pdbs/pattern_selection/cegar/flaw_finding_strategy_factory.h"

namespace options {
class Options;
}

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
class PUCSFlawFinderFactory : public FlawFindingStrategyFactory<PDBType> {
    unsigned int violation_threshold;

public:
    PUCSFlawFinderFactory(const options::Options& opts);
    PUCSFlawFinderFactory(unsigned int violation_threshold);
    ~PUCSFlawFinderFactory() override = default;

    std::unique_ptr<FlawFindingStrategy<PDBType>>
    create_flaw_finder(const ProbabilisticTask* task) override;

    std::string get_name() const override { return "PUCS Flaw Finder Factory"; }
};

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif