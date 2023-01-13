#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_SAMPLING_FLAW_FINDER_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_SAMPLING_FLAW_FINDER_H

#include "probfd/heuristics/pdbs/cegar/flaw_finding_strategy.h"

#include "probfd/distribution.h"

#include <stack>
#include <unordered_map>

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace cegar {

template <typename PDBType>
class SamplingFlawFinder : public FlawFindingStrategy<PDBType> {
    struct ExplorationInfo {
        Distribution<std::vector<int>> successors;
    };

    std::stack<std::vector<int>> stk;
    std::unordered_map<
        std::vector<int>,
        ExplorationInfo,
        typename FlawFindingStrategy<PDBType>::StateHash>
        einfos;

    unsigned int violation_threshold;

    static constexpr unsigned int STATE_PUSHED = 1;
    static constexpr unsigned int FLAW_OCCURED = 1 << 1;

public:
    SamplingFlawFinder(
        const ProbabilisticTask* task,
        unsigned int violation_threshold);

    ~SamplingFlawFinder() override = default;

    virtual bool apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        std::vector<int>& state,
        std::vector<Flaw>& flaw_list) override;

private:
    unsigned int push_state(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        std::vector<int>& state,
        std::vector<Flaw>& flaw_list);
};

} // namespace cegar
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif