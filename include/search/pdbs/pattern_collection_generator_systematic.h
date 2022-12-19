#ifndef PDBS_PATTERN_COLLECTION_GENERATOR_SYSTEMATIC_H
#define PDBS_PATTERN_COLLECTION_GENERATOR_SYSTEMATIC_H

#include "pdbs/pattern_generator.h"
#include "pdbs/types.h"

#include "utils/hash.h"

#include <cstdlib>
#include <memory>
#include <unordered_set>
#include <vector>

namespace causal_graph{
class CausalGraph;
}

namespace options {
class Options;
}

namespace pdbs {
class CanonicalPDBsHeuristic;

// Invariant: patterns are always sorted.
class PatternCollectionGeneratorSystematic : public PatternCollectionGenerator {
    using PatternSet = utils::HashSet<Pattern>;

    const size_t max_pattern_size;
    const bool only_interesting_patterns;
    std::shared_ptr<PatternCollection> patterns;
    PatternSet pattern_set; // Cleared after pattern computation.

    void enqueue_pattern_if_new(const Pattern& pattern);
    void compute_eff_pre_neighbors(
        const causal_graph::CausalGraph& cg,
        const Pattern& pattern,
        std::vector<int>& result) const;
    void compute_connection_points(
        const causal_graph::CausalGraph& cg,
        const Pattern& pattern,
        std::vector<int>& result) const;

    void build_sga_patterns(const causal_graph::CausalGraph& cg);
    void build_patterns();
    void build_patterns_naive();

public:
    explicit PatternCollectionGeneratorSystematic(const options::Options& opts);

    virtual PatternCollectionInformation
    generate(OperatorCost cost_type) override;
};
} // namespace pdbs

#endif
