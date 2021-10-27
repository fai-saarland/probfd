#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_PUCS_FLAW_FINDER_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_PUCS_FLAW_FINDER_H

#include "flaw_finding_strategy.h"

#include "../../../../value_type.h"

#include "../../../../../algorithms/priority_queues.h"

#include <stack>
#include <unordered_map>

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
class PUCSFlawFinder : public FlawFindingStrategy<PDBType> {
    priority_queues::HeapQueue<value_type::value_t, ExplicitGState> pq;
    std::unordered_map<ExplicitGState, value_type::value_t> probabilities;

    static constexpr unsigned int SUCCESSFULLY_EXPANDED = 1;
    static constexpr unsigned int GOAL_VIOLATED = 1 << 1;

public:
    ~PUCSFlawFinder() override = default;

    virtual std::pair<FlawList, bool> apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        const ExplicitGState& init) override;

    std::string get_name() const override {
        return "PUCS Flaw Finder";
    }

private:
    unsigned int expand(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        ExplicitGState state,
        value_type::value_t priority,
        FlawList& flaw_list);
};

}
}
}

#endif