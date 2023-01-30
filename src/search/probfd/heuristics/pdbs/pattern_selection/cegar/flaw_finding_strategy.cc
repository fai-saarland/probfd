#include "probfd/heuristics/pdbs/pattern_selection/cegar/flaw_finding_strategy.h"

#include "probfd/heuristics/pdbs/expcost_projection.h"
#include "probfd/heuristics/pdbs/maxprob_projection.h"

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
FlawFindingStrategy<PDBType>::FlawFindingStrategy(const ProbabilisticTask* task)
    : task(task)
    , task_proxy(*task)
{
}

template class FlawFindingStrategy<MaxProbProjection>;
template class FlawFindingStrategy<ExpCostProjection>;

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd