#ifndef PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_H
#define PDBS_PATTERN_COLLECTION_GENERATOR_DISJOINT_CEGAR_H

#include "downward/pdbs/pattern_generator.h"

namespace downward::utils {
class RandomNumberGenerator;
}

namespace downward::pdbs {
/*
  This pattern collection generator uses the CEGAR algorithm to compute a
  disjoint pattern collection for the given task. See cegar.h for more details.
*/
class PatternCollectionGeneratorDisjointCegar
    : public PatternCollectionGenerator {
    const int max_pdb_size;
    const int max_collection_size;
    const double max_time;
    const bool use_wildcard_plans;
    std::shared_ptr<utils::RandomNumberGenerator> rng;

    std::string name() const override;
    PatternCollectionInformation
    compute_patterns(const SharedAbstractTask& task) override;

public:
    PatternCollectionGeneratorDisjointCegar(
        int max_pdb_size,
        int max_collection_size,
        double max_time,
        bool use_wildcard_plans,
        int random_seed,
        utils::Verbosity verbosity);
};
} // namespace downward::pdbs

#endif
