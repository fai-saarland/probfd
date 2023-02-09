#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/heuristics/pdbs/maxprob_pattern_database.h"
#include "probfd/heuristics/pdbs/ssp_pattern_database.h"


namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

template <typename PDBType>
FlawFindingStrategy<PDBType>::FlawFindingStrategy(const ProbabilisticTask* task)
    : task(task)
    , task_proxy(*task)
{
}

template class FlawFindingStrategy<MaxProbPatternDatabase>;
template class FlawFindingStrategy<SSPPatternDatabase>;

}
} // namespace pdbs
} // namespace heuristics
} // namespace probfd