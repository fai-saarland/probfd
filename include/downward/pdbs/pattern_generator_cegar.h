#ifndef PDBS_PATTERN_GENERATOR_CEGAR_H
#define PDBS_PATTERN_GENERATOR_CEGAR_H

#include "downward/pdbs/pattern_generator.h"

namespace downward::utils {
class RandomNumberGenerator;
}

namespace downward::pdbs {
class PatternGeneratorCEGAR : public PatternGenerator {
    const int max_pdb_size;
    const double max_time;
    const bool use_wildcard_plans;
    std::shared_ptr<utils::RandomNumberGenerator> rng;

    std::string name() const override;
    PatternInformation compute_pattern(const SharedAbstractTask& task) override;

public:
    PatternGeneratorCEGAR(
        int max_pdb_size,
        double max_time,
        bool use_wildcard_plans,
        int random_seed,
        utils::Verbosity verbosity);
};
} // namespace downward::pdbs

#endif
