#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_PUCS_FLAW_FINDER_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_PUCS_FLAW_FINDER_H

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/value_type.h"

#include "algorithms/priority_queues.h"

#include "task_proxy.h"

#include <stack>
#include <unordered_map>

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

template <typename PDBType>
class PUCSFlawFinder : public FlawFindingStrategy<PDBType> {
    priority_queues::HeapQueue<value_t, std::vector<int>> pq;
    std::unordered_map<
        std::vector<int>,
        value_t,
        typename FlawFindingStrategy<PDBType>::StateHash>
        probabilities;

    unsigned int violation_threshold;

public:
    PUCSFlawFinder(
        const ProbabilisticTask* task,
        unsigned int violation_threshold);
    ~PUCSFlawFinder() override = default;

    virtual bool apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        std::vector<int>& init,
        std::vector<Flaw>& flaw_list) override;

private:
    bool expand(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        std::vector<int>& state,
        value_t priority,
        std::vector<Flaw>& flaw_list);
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif