#ifndef PDBS_PATTERN_COLLECTION_GENERATOR_MANUAL_H
#define PDBS_PATTERN_COLLECTION_GENERATOR_MANUAL_H

#include "downward/pdbs/pattern_generator.h"
#include "downward/pdbs/types.h"

#include <memory>

namespace downward::pdbs {
class PatternCollectionGeneratorManual : public PatternCollectionGenerator {
    std::shared_ptr<PatternCollection> patterns;

    std::string name() const override;
    PatternCollectionInformation
    compute_patterns(const SharedAbstractTask& task) override;

public:
    explicit PatternCollectionGeneratorManual(
        const std::vector<Pattern>& patterns,
        utils::Verbosity verbosity);
};
} // namespace downward::pdbs

#endif
