#ifndef PDBS_PATTERN_COLLECTION_GENERATOR_COMBO_H
#define PDBS_PATTERN_COLLECTION_GENERATOR_COMBO_H

#include "downward/pdbs/pattern_generator.h"

namespace downward::pdbs {
/* Take one large pattern and then single-variable patterns for
   all goal variables that are not in the large pattern. */
class PatternCollectionGeneratorCombo : public PatternCollectionGenerator {
    int max_states;
    utils::Verbosity verbosity;

    std::string name() const override;
    PatternCollectionInformation
    compute_patterns(const SharedAbstractTask& task) override;

public:
    PatternCollectionGeneratorCombo(int max_states, utils::Verbosity verbosity);
};
} // namespace pdbs

#endif
