#ifndef PDBS_PATTERN_COLLECTION_GENERATOR_COMBO_H
#define PDBS_PATTERN_COLLECTION_GENERATOR_COMBO_H

#include "pdbs/pattern_generator.h"

namespace options {
class Options;
}

namespace pdbs {
/* Take one large pattern and then single-variable patterns for
   all goal variables that are not in the large pattern. */
class PatternCollectionGeneratorCombo : public PatternCollectionGenerator {
    int max_states;
public:
    explicit PatternCollectionGeneratorCombo(const options::Options &opts);
    virtual ~PatternCollectionGeneratorCombo() = default;

    virtual PatternCollectionInformation generate(OperatorCost cost_type) override;
};
}

#endif
