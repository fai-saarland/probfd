#ifndef PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_GENERATOR_DETERMINISTIC_H
#define PROBFD_HEURISTICS_PDBS_PATTERN_COLLECTION_GENERATOR_DETERMINISTIC_H

#include "pdbs/pattern_generator.h"

#include "probfd/heuristics/pdbs/pattern_generator.h"

#include <memory>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {
namespace heuristics {
namespace pdbs {

class SubCollectionFinderFactory;

template <class PDBType>
class PatternCollectionGeneratorDeterministic
    : public PatternCollectionGenerator<PDBType> {
    std::shared_ptr<::pdbs::PatternCollectionGenerator> gen;
    std::shared_ptr<SubCollectionFinderFactory> finder_factory;

    PatternCollectionGeneratorDeterministic(
        std::shared_ptr<::pdbs::PatternCollectionGenerator> gen,
        std::shared_ptr<SubCollectionFinderFactory> finder);

public:
    PatternCollectionGeneratorDeterministic(const options::Options& opts);
    ~PatternCollectionGeneratorDeterministic() override = default;

    PatternCollectionInformation<PDBType>
    generate(const std::shared_ptr<ProbabilisticTask>& task) override;

    std::shared_ptr<utils::Printable> get_report() const override;
};

using ExpCostPatternCollectionGeneratorDeterministic =
    PatternCollectionGeneratorDeterministic<SSPPatternDatabase>;

using MaxProbPatternCollectionGeneratorDeterministic =
    PatternCollectionGeneratorDeterministic<MaxProbPatternDatabase>;

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PATTERN_COLLECTION_GENERATOR_DETERMINISTIC_H_