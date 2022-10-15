#ifndef PDBS_PATTERN_COLLECTION_GENERATOR_MANUAL_H
#define PDBS_PATTERN_COLLECTION_GENERATOR_MANUAL_H

#include "pdbs/pattern_generator.h"
#include "pdbs/types.h"

#include <memory>

namespace options {
class Options;
}

namespace pdbs {
class PatternCollectionGeneratorManual : public PatternCollectionGenerator {
    std::shared_ptr<PatternCollection> patterns;
public:
    explicit PatternCollectionGeneratorManual(const options::Options &opts);
    virtual ~PatternCollectionGeneratorManual() = default;

    virtual PatternCollectionInformation generate(OperatorCost cost_type) override;
};
}

#endif
