#ifndef PDBS_PATTERN_COLLECTION_GENERATOR_FAST_CEGAR_H
#define PDBS_PATTERN_COLLECTION_GENERATOR_FAST_CEGAR_H

#include "pdbs/pattern_generator.h"

#include "utils/logging.h"

namespace options {
class Options;
}

namespace pdbs {

class PatternCollectionGeneratorFastCegar : public PatternCollectionGenerator {
    const int single_generator_max_refinements;
    const int single_generator_max_pdb_size;
    const int single_generator_max_collection_size; // Possibly overwritten by
                                                    // total_collection_max_size
    const bool single_generator_extended_plans;
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
    explicit PatternCollectionGeneratorFastCegar(options::Options& opts);
    virtual ~PatternCollectionGeneratorFastCegar() = default;

    virtual PatternCollectionInformation
    generate(OperatorCost cost_type) override;
};
} // namespace pdbs

#endif
