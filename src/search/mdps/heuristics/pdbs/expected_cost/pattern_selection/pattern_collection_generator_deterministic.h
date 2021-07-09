#ifndef PATTERN_COLLECTION_GENERATOR_DETERMINISTIC_H_
#define PATTERN_COLLECTION_GENERATOR_DETERMINISTIC_H_

#include "pattern_generator.h"
#include "../../../../../pdbs/pattern_generator.h"

#include "../../../../../options/options.h"

#include <memory>

namespace probabilistic {
namespace pdbs {
namespace pattern_selection {

class PatternCollectionGeneratorDeterministic
    : public PatternCollectionGenerator
{
    std::shared_ptr<::pdbs::PatternCollectionGenerator> gen;
    std::shared_ptr<additivity::AdditivityStrategy> additivity;

    PatternCollectionGeneratorDeterministic(
        std::shared_ptr<::pdbs::PatternCollectionGenerator> gen,
        std::shared_ptr<additivity::AdditivityStrategy> additivity);

public:
    PatternCollectionGeneratorDeterministic(options::Options& opts);
    
    virtual ~PatternCollectionGeneratorDeterministic() override = default;

    virtual PatternCollectionInformation
    generate(OperatorCost cost_type) override;

    std::shared_ptr<utils::Printable> get_report() const override;
};

}
}
}

#endif // PATTERN_COLLECTION_GENERATOR_DETERMINISTIC_H_