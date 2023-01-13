#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_BFS_FLAW_FINDER_FACTORY_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_BFS_FLAW_FINDER_FACTORY_H

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy_factory.h"

#include "probfd/task_proxy.h"

namespace options {
class Options;
}

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

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

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif