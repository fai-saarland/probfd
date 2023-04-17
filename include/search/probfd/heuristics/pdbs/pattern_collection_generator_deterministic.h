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

class PatternCollectionGeneratorDeterministic
    : public PatternCollectionGenerator {
    std::shared_ptr<::pdbs::PatternCollectionGenerator> gen;
    std::shared_ptr<SubCollectionFinderFactory> finder_factory;

    PatternCollectionGeneratorDeterministic(
        const utils::LogProxy& log,
        std::shared_ptr<::pdbs::PatternCollectionGenerator> gen,
        std::shared_ptr<SubCollectionFinderFactory> finder);

public:
    PatternCollectionGeneratorDeterministic(const options::Options& opts);
    ~PatternCollectionGeneratorDeterministic() override = default;

    PatternCollectionInformation
    generate(const std::shared_ptr<ProbabilisticTask>& task) override;

    std::shared_ptr<utils::Printable> get_report() const override;
};

} // namespace pdbs
} // namespace heuristics
} // namespace probfd

#endif // PATTERN_COLLECTION_GENERATOR_DETERMINISTIC_H_