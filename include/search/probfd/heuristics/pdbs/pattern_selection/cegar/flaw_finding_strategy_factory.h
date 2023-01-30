#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_FLAW_FINDER_FACTORY_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_FLAW_FINDER_FACTORY_H

#include "probfd/heuristics/pdbs/pattern_selection/cegar/types.h"

#include "probfd/task_proxy.h"

#include <memory>

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

template <typename>
class FlawFindingStrategy;

template <typename PDBType>
class FlawFindingStrategyFactory {
public:
    virtual ~FlawFindingStrategyFactory() = default;

    virtual std::unique_ptr<FlawFindingStrategy<PDBType>>
    create_flaw_finder(const ProbabilisticTask* task) = 0;

    virtual std::string get_name() const = 0;
};

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif