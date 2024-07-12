#ifndef PDBS_PATTERN_GENERATOR_MANUAL_H
#define PDBS_PATTERN_GENERATOR_MANUAL_H

#include "downward/pdbs/pattern_generator.h"
#include "downward/pdbs/types.h"

namespace pdbs {
class PatternGeneratorManual : public PatternGenerator {
    Pattern pattern;

    virtual std::string name() const override;
    virtual PatternInformation
    compute_pattern(const std::shared_ptr<AbstractTask>& task) override;

public:
    PatternGeneratorManual(
        const std::vector<int>& pattern,
        utils::Verbosity verbosity);
};
} // namespace pdbs

#endif
