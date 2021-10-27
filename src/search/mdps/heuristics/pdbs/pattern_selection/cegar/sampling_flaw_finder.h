#ifndef MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_SAMPLING_FLAW_FINDER_H
#define MDPS_HEURISTICS_PDBS_PATTERN_SELECTION_CEGAR_SAMPLING_FLAW_FINDER_H

#include "flaw_finding_strategy.h"
#include "../../../../distribution.h"

#include <stack>
#include <unordered_map>

namespace options {
class Options;
}

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

template <typename PDBType>
class SamplingFlawFinder : public FlawFindingStrategy<PDBType> {
    struct ExplorationInfo {
        Distribution<ExplicitGState> successors;
    };

    std::stack<ExplicitGState> stk;
    std::unordered_map<ExplicitGState, ExplorationInfo> einfos;

    unsigned int violation_threshold;

    static constexpr unsigned int STATE_PUSHED = 1;
    static constexpr unsigned int FLAW_OCCURED = 1 << 1;

public:
    SamplingFlawFinder(options::Options& parser);

    SamplingFlawFinder(unsigned int violation_threshold = 1);
    ~SamplingFlawFinder() override = default;

    virtual std::pair<FlawList, bool> apply_policy(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        const ExplicitGState& init) override;

    std::string get_name() const override {
        return "Sampling Flaw Finder";
    }

private:
    unsigned int push_state(
        PatternCollectionGeneratorCegar<PDBType>& base,
        int solution_index,
        const ExplicitGState& state,
        FlawList& flaw_list);
};

}
}
}

#endif