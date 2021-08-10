#pragma once

#include "../types.h"
#include "subcollection_finder.h"

namespace probabilistic {
namespace pdbs {

class MaxOrthogonalityFinder : public SubCollectionFinder {
    const VariableOrthogonality var_orthogonality;

public:
    MaxOrthogonalityFinder();

    std::shared_ptr<std::vector<PatternSubCollection>>
    compute_subcollections(const PatternCollection&) override;

    virtual std::vector<PatternSubCollection>
    compute_subcollections_with_pattern(
        const PatternCollection& patterns,
        const std::vector<PatternSubCollection>& known_pattern_cliques,
        const Pattern& new_pattern) override;
};

} // namespace pdbs
} // namespace probabilistic