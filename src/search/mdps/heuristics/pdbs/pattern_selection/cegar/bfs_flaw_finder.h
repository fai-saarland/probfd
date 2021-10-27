#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_BFS_FLAW_FINDER_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_BFS_FLAW_FINDER_H

#include "flaw_finding_strategy.h"

#include <stack>
#include <unordered_set>

namespace options {
class Options;
}

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
class BFSFlawFinder : public FlawFindingStrategy<PDBType> {
    std::deque<ExplicitGState> open;
    std::unordered_set<ExplicitGState> closed;

    unsigned int violation_threshold;

public:
    BFSFlawFinder(options::Options& parser);

    BFSFlawFinder(unsigned int violation_threshold = 1);
    ~BFSFlawFinder() override = default;

    virtual std::pair<FlawList, bool> apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        const ExplicitGState& init) override;

    std::string get_name() const override {
        return "BFS Flaw Finder";
    }

private:
    bool expand(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        ExplicitGState state,
        FlawList& flaw_list);
};

}
}
}

#endif