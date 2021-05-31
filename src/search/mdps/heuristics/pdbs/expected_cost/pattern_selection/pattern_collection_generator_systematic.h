#pragma once

#include "pattern_generator.h"
#include "../../types.h"

#include "../../../../../utils/hash.h"

#include <cstdlib>
#include <memory>
#include <unordered_set>
#include <vector>

class CausalGraph;

namespace options {
class Options;
}

namespace probabilistic {
namespace pdbs {

namespace additivity {
class AdditivityStrategy;
}

namespace pattern_selection {

// Invariant: patterns are always sorted.
class PatternCollectionGeneratorSystematic : public PatternCollectionGenerator {
    using PatternSet = utils::HashSet<Pattern>;

    const size_t max_pattern_size;
    const bool only_interesting_patterns;
    const std::shared_ptr<additivity::AdditivityStrategy> additivity_strategy;

    std::shared_ptr<PatternCollection> patterns;
    PatternSet pattern_set;  // Cleared after pattern computation.

    void enqueue_pattern_if_new(const Pattern &pattern);
    void compute_eff_pre_neighbors(const CausalGraph &cg,
                                   const Pattern &pattern,
                                   std::vector<int> &result) const;
    void compute_connection_points(const CausalGraph &cg,
                                   const Pattern &pattern,
                                   std::vector<int> &result) const;

    void build_sga_patterns(const CausalGraph &cg);
    void build_patterns();
    void build_patterns_naive();
public:
    explicit PatternCollectionGeneratorSystematic(const options::Options &opts);

    PatternCollectionInformation generate(OperatorCost cost_type) override;
};

}
}
}
