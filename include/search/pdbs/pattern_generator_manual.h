#ifndef PDBS_PATTERN_GENERATOR_MANUAL_H
#define PDBS_PATTERN_GENERATOR_MANUAL_H

#include "pdbs/pattern_generator.h"
#include "pdbs/types.h"

namespace options {
class Options;
}

namespace pdbs {
class PatternGeneratorManual : public PatternGenerator {
    Pattern pattern;
public:
    explicit PatternGeneratorManual(const options::Options &opts);
    virtual ~PatternGeneratorManual() = default;

    virtual PatternInformation generate(OperatorCost cost_type) override;
};
}

#endif
