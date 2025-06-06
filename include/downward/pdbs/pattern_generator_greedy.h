#ifndef PDBS_PATTERN_GENERATOR_GREEDY_H
#define PDBS_PATTERN_GENERATOR_GREEDY_H

#include "downward/pdbs/pattern_generator.h"

namespace downward::pdbs {
class PatternGeneratorGreedy : public PatternGenerator {
    const int max_states;

    virtual std::string name() const override;
    virtual PatternInformation
    compute_pattern(const std::shared_ptr<AbstractTask>& task) override;

public:
    PatternGeneratorGreedy(int max_states, utils::Verbosity verbosity);
};
} // namespace pdbs

#endif
