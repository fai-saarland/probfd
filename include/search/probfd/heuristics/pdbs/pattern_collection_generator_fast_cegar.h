#ifndef PROBFD_HEURISTICS_PDBS_CEGAR_PATTERN_COLLECTION_GENERATOR_FAST_CEGAR_H
#define PROBFD_HEURISTICS_PDBS_CEGAR_PATTERN_COLLECTION_GENERATOR_FAST_CEGAR_H

#include "probfd/heuristics/pdbs/pattern_generator.h"

#include "utils/logging.h"

namespace options {
class Options;
}

namespace probfd {
namespace heuristics {
namespace pdbs {

class SubCollectionFinderFactory;

namespace cegar {
template <typename PDBType>
class FlawFindingStrategyFactory;
}

template <typename PDBType>
class PatternCollectionGeneratorFastCegar
    : public PatternCollectionGenerator<PDBType> {
    // Subcollection finder
    std::shared_ptr<SubCollectionFinderFactory> subcollection_finder_factory;

    // Flaw finding strategy
    std::shared_ptr<cegar::FlawFindingStrategyFactory<PDBType>>
        flaw_strategy_factory;

    const int single_generator_max_refinements;
    const int single_generator_max_pdb_size;
    const int single_generator_max_collection_size; // Possibly overwritten by
                                                    // total_collection_max_size
    const bool single_generator_wildcard_policies;
    const bool single_generator_treat_goal_violations_differently;
    const bool single_generator_local_blacklisting;
    const double single_generator_max_time; // Possibly overwritten by
                                            // remaining total_time_limit
    const utils::Verbosity single_generator_verbosity;

    const int initial_random_seed;
    const int total_collection_max_size;
    const double stagnation_limit;
    const double blacklist_trigger_time;
    const bool blacklist_on_stagnation;
    const double total_time_limit;

public:
    explicit PatternCollectionGeneratorFastCegar(const options::Options& opts);
    ~PatternCollectionGeneratorFastCegar() override = default;

    PatternCollectionInformation<PDBType>
    generate(const std::shared_ptr<ProbabilisticTask>& task) override;
};

using ExpCostPatternCollectionGeneratorFastCegar =
    PatternCollectionGeneratorFastCegar<SSPPatternDatabase>;
using MaxProbPatternCollectionGeneratorFastCegar =
    PatternCollectionGeneratorFastCegar<MaxProbPatternDatabase>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif
