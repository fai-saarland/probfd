#ifndef PDBS_PATTERN_GENERATOR_RANDOM_H
#define PDBS_PATTERN_GENERATOR_RANDOM_H

#include "downward/pdbs/pattern_generator.h"

namespace downward::utils {
class RandomNumberGenerator;
}

namespace downward::pdbs {
class PatternGeneratorRandom : public PatternGenerator {
    const int max_pdb_size;
    const utils::FSeconds max_time;
    const bool bidirectional;
    std::shared_ptr<utils::RandomNumberGenerator> rng;

    std::string name() const override;
    PatternInformation compute_pattern(const SharedAbstractTask& task) override;

public:
    PatternGeneratorRandom(
        int max_pdb_size,
        utils::FSeconds max_time,
        bool bidirectional,
        std::shared_ptr<utils::RandomNumberGenerator> rng,
        utils::Verbosity verbosity);
};
} // namespace downward::pdbs

#endif
