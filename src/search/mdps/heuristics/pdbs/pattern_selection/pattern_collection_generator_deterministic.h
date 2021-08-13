#ifndef PATTERN_COLLECTION_GENERATOR_DETERMINISTIC_H_
#define PATTERN_COLLECTION_GENERATOR_DETERMINISTIC_H_

#include "../../../../pdbs/pattern_generator.h"
#include "pattern_generator.h"

#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

template <class PDBType>
class PatternCollectionGeneratorDeterministic
    : public PatternCollectionGenerator<PDBType> {
    std::shared_ptr<::pdbs::PatternCollectionGenerator> gen;
    std::shared_ptr<SubCollectionFinder> finder;

    PatternCollectionGeneratorDeterministic(
        std::shared_ptr<::pdbs::PatternCollectionGenerator> gen,
        std::shared_ptr<SubCollectionFinder> finder);

public:
    PatternCollectionGeneratorDeterministic(options::Options& opts);

    virtual ~PatternCollectionGeneratorDeterministic() override = default;

    virtual PatternCollectionInformation<PDBType>
    generate(OperatorCost cost_type) override;

    std::shared_ptr<utils::Printable> get_report() const override;
};

using ExpCostPatternCollectionGeneratorDeterministic =
    PatternCollectionGeneratorDeterministic<ExpCostProjection>;

using MaxProbPatternCollectionGeneratorDeterministic =
    PatternCollectionGeneratorDeterministic<MaxProbProjection>;

} // namespace pattern_selection
} // namespace pdbs
} // namespace probabilistic

#endif // PATTERN_COLLECTION_GENERATOR_DETERMINISTIC_H_