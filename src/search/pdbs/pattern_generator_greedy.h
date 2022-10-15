#ifndef PDBS_PATTERN_GENERATOR_GREEDY_H
#define PDBS_PATTERN_GENERATOR_GREEDY_H

#include "pdbs/pattern_generator.h"

namespace options {
class Options;
}

namespace pdbs {
class PatternGeneratorGreedy : public PatternGenerator {
    int max_states;

public:
    explicit PatternGeneratorGreedy(const options::Options& opts);
    explicit PatternGeneratorGreedy(int max_states);
    virtual ~PatternGeneratorGreedy() = default;

    virtual PatternInformation generate(OperatorCost cost_type) override;
};
} // namespace pdbs

#endif
