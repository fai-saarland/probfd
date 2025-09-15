#ifndef PROBFD_TASK_UTILS_PROBABILISTIC_SUCCESSOR_GENERATOR_FACTORY_H
#define PROBFD_TASK_UTILS_PROBABILISTIC_SUCCESSOR_GENERATOR_FACTORY_H

#include <memory>
#include <utility>
#include <vector>

// Forward Declarations
namespace downward {
class VariableSpace;
class OperatorSpace;
} // namespace downward

namespace probfd::successor_generator {
class ProbabilisticGeneratorBase;
struct OperatorRange;
class OperatorInfo;
} // namespace probfd::successor_generator

namespace probfd::successor_generator {

using GeneratorPtr = std::unique_ptr<ProbabilisticGeneratorBase>;

class ProbabilisticSuccessorGeneratorFactory {
    using ValuesAndGenerators = std::vector<std::pair<int, GeneratorPtr>>;

    const downward::VariableSpace& variables_;
    const downward::OperatorSpace& operators_;

    std::vector<OperatorInfo> operator_infos_;

    [[nodiscard]]
    GeneratorPtr construct_leaf(OperatorRange range) const;

    [[nodiscard]]
    GeneratorPtr construct_switch(
        int switch_var_id,
        ValuesAndGenerators values_and_generators) const;

    [[nodiscard]]
    GeneratorPtr construct_recursive(int depth, OperatorRange range) const;

public:
    explicit ProbabilisticSuccessorGeneratorFactory(
        const downward::VariableSpace& variables,
        const downward::OperatorSpace& operators);

    // Destructor cannot be implicit because OperatorInfo is forward-declared.
    ~ProbabilisticSuccessorGeneratorFactory();

    GeneratorPtr create();
};

} // namespace probfd::successor_generator

#endif
