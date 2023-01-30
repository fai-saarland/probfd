#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_SAMPLING_FLAW_FINDER_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_SAMPLING_FLAW_FINDER_H

#include "probfd/heuristics/pdbs/pattern_selection/cegar/flaw_finding_strategy.h"

#include "probfd/distribution.h"

#include <stack>
#include <unordered_map>

namespace probfd {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

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

    virtual std::pair<FlawList, bool> apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        std::vector<int>& state) override;

private:
    unsigned int push_state(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        std::vector<int>& state,
        FlawList& flaw_list);
};

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif