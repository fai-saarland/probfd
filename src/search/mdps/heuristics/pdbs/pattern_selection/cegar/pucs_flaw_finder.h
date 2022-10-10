#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_PUCS_FLAW_FINDER_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_PUCS_FLAW_FINDER_H

#include "flaw_finding_strategy.h"

#include "../../../../value_type.h"

#include "../../../../../algorithms/priority_queues.h"

#include <stack>
#include <unordered_map>

namespace options {
class Options;
}

namespace probabilistic {
namespace heuristics {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
class PUCSFlawFinder : public FlawFindingStrategy<PDBType> {
    priority_queues::HeapQueue<value_type::value_t, ExplicitGState> pq;
    std::unordered_map<ExplicitGState, value_type::value_t> probabilities;

    unsigned int violation_threshold;

public:
    PUCSFlawFinder(options::Options& parser);

    PUCSFlawFinder(unsigned int violation_threshold = 1);
    ~PUCSFlawFinder() override = default;

    virtual std::pair<FlawList, bool> apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        const ExplicitGState& init) override;

    std::string get_name() const override { return "PUCS Flaw Finder"; }

private:
    bool expand(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        ExplicitGState state,
        value_type::value_t priority,
        FlawList& flaw_list);
};

} // namespace pattern_selection
} // namespace pdbs
} // namespace heuristics
} // namespace probabilistic

#endif