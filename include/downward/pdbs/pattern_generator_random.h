#ifndef PDBS_PATTERN_GENERATOR_RANDOM_H
#define PDBS_PATTERN_GENERATOR_RANDOM_H

#include "downward/pdbs/pattern_generator.h"

namespace downward::utils {
class RandomNumberGenerator;
}

namespace downward::pdbs {
class PatternGeneratorRandom : public PatternGenerator {
    const int max_pdb_size;
    const double max_time;
    const bool bidirectional;
    std::shared_ptr<utils::RandomNumberGenerator> rng;

    virtual std::string name() const override;
    virtual PatternInformation
    compute_pattern(const std::shared_ptr<AbstractTask>& task) override;

public:
    PatternGeneratorRandom(
        int max_pdb_size,
        double max_time,
        bool bidirectional,
        int random_seed,
        utils::Verbosity verbosity);
};
} // namespace pdbs

#endif
