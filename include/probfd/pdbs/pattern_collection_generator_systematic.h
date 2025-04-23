#ifndef PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_SYSTEMATIC_H
#define PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_SYSTEMATIC_H

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/types.h"

#include "downward/utils/hash.h"

#include <cstdlib>
#include <memory>
#include <unordered_set>
#include <vector>

namespace probfd {
class ProbabilisticTaskProxy;
class ProbabilisticTask;
} // namespace probfd

namespace probfd::causal_graph {
class ProbabilisticCausalGraph;
}

namespace probfd::pdbs {

class PatternCollectionGeneratorSystematic : public PatternCollectionGenerator {
    using PatternSet = downward::utils::HashSet<Pattern>;

    const size_t max_pattern_size;
    const bool only_interesting_patterns;

public:
    PatternCollectionGeneratorSystematic(
        int pattern_max_size,
        bool only_interesting_patterns,
        downward::utils::Verbosity verbosity);

    PatternCollectionInformation
    generate(const std::shared_ptr<ProbabilisticTask>& task) override;

private:
    void enqueue_pattern_if_new(
        const Pattern& pattern,
        PatternCollection& patterns,
        PatternSet& pattern_set);

    void build_sga_patterns(
        const ProbabilisticTask& task,
        const causal_graph::ProbabilisticCausalGraph& cg,
        PatternCollection& patterns,
        PatternSet& pattern_set);
    void build_patterns(
        const ProbabilisticTask& task,
        PatternCollection& patterns);
    void build_patterns_naive(
        const ProbabilisticTask& task,
        PatternCollection& patterns);
};

} // namespace probfd::pdbs

#endif // PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_SYSTEMATIC_H
