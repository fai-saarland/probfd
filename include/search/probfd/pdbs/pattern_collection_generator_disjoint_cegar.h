#ifndef PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_H
#define PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_H

#include "probfd/pdbs/pattern_collection_generator.h"
#include "probfd/pdbs/types.h"

#include "downward/plugins/options.h"

#include "downward/utils/rng.h"

namespace plugins {
class Feature;
class Options;
} // namespace plugins

namespace probfd {
namespace pdbs {

class SubCollectionFinderFactory;

namespace cegar {
class FlawFindingStrategy;
} // namespace cegar

class PatternCollectionGeneratorDisjointCegar
    : public PatternCollectionGenerator {
    const bool use_wildcard_policies;
    const bool single_goal;
    const int max_pdb_size;
    const int max_collection_size;
    const double max_time;
    std::shared_ptr<utils::RandomNumberGenerator> rng;
    std::shared_ptr<SubCollectionFinderFactory> subcollection_finder_factory;
    std::shared_ptr<cegar::FlawFindingStrategy> flaw_strategy;

public:
    explicit PatternCollectionGeneratorDisjointCegar(
        const plugins::Options& opts);

    virtual ~PatternCollectionGeneratorDisjointCegar() = default;

    PatternCollectionInformation generate(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) override;
};

void add_pattern_collection_generator_cegar_options_to_feature(
    plugins::Feature& feature);

} // namespace pdbs
} // namespace probfd

#endif // PROBFD_PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_H
