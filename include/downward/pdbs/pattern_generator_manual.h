#ifndef PDBS_PATTERN_GENERATOR_MANUAL_H
#define PDBS_PATTERN_GENERATOR_MANUAL_H

#include "downward/pdbs/pattern_generator.h"
#include "downward/pdbs/types.h"

namespace downward::pdbs {
class PatternGeneratorManual : public PatternGenerator {
    Pattern pattern;

    std::string name() const override;
    PatternInformation compute_pattern(const SharedAbstractTask& task) override;

public:
    PatternGeneratorManual(
        const std::vector<int>& pattern,
        utils::Verbosity verbosity);
};
} // namespace downward::pdbs

#endif
