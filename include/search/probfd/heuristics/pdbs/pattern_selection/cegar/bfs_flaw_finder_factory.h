#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_BFS_FLAW_FINDER_FACTORY_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_BFS_FLAW_FINDER_FACTORY_H

#include "probfd/heuristics/pdbs/pattern_selection/cegar/flaw_finding_strategy_factory.h"

#include "probfd/task_proxy.h"

namespace options {
class Options;
}

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
class BFSFlawFinderFactory : public FlawFindingStrategyFactory<PDBType> {
    unsigned int violation_threshold;

public:
    BFSFlawFinderFactory(options::Options& opts);
    BFSFlawFinderFactory(unsigned int violation_threshold);

    ~BFSFlawFinderFactory() override = default;

    std::unique_ptr<FlawFindingStrategy<PDBType>>
    create_flaw_finder(const ProbabilisticTask* task) override;

    std::string get_name() const override { return "BFS Flaw Finder Factory"; }
};

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif