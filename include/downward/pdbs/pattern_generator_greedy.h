#ifndef PDBS_PATTERN_GENERATOR_GREEDY_H
#define PDBS_PATTERN_GENERATOR_GREEDY_H

#include "downward/pdbs/pattern_generator.h"

namespace downward::pdbs {
class PatternGeneratorGreedy : public PatternGenerator {
    const int max_states;

    std::string name() const override;
    PatternInformation compute_pattern(const SharedAbstractTask& task) override;

public:
    PatternGeneratorGreedy(int max_states, utils::Verbosity verbosity);
};
} // namespace downward::pdbs

#endif
