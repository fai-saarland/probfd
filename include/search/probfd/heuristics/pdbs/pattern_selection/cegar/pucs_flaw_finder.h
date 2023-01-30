#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_PUCS_FLAW_FINDER_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_PUCS_FLAW_FINDER_H

#include "probfd/heuristics/pdbs/pattern_selection/cegar/flaw_finding_strategy.h"

#include "probfd/value_type.h"

#include "algorithms/priority_queues.h"

#include "task_proxy.h"

#include <stack>
#include <unordered_map>

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
class PUCSFlawFinder : public FlawFindingStrategy<PDBType> {
    priority_queues::HeapQueue<value_type::value_t, std::vector<int>> pq;
    std::unordered_map<
        std::vector<int>,
        value_type::value_t,
        typename FlawFindingStrategy<PDBType>::StateHash>
        probabilities;

    unsigned int violation_threshold;

public:
    PUCSFlawFinder(
        const ProbabilisticTask* task,
        unsigned int violation_threshold);
    ~PUCSFlawFinder() override = default;

    virtual std::pair<FlawList, bool> apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        std::vector<int>& init) override;

private:
    bool expand(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        std::vector<int>& state,
        value_type::value_t priority,
        FlawList& flaw_list);
};

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif