#ifndef TASK_UTILS_SUCCESSOR_GENERATOR_FACTORY_H
#define TASK_UTILS_SUCCESSOR_GENERATOR_FACTORY_H

#include <memory>
#include <vector>

namespace downward {
class VariableSpace;
class OperatorSpace;
} // namespace downward

namespace downward::successor_generator {
class GeneratorBase;

using GeneratorPtr = std::unique_ptr<GeneratorBase>;

struct OperatorRange;
class OperatorInfo;

class SuccessorGeneratorFactory {
    using ValuesAndGenerators = std::vector<std::pair<int, GeneratorPtr>>;

    const VariableSpace& variables;
    const OperatorSpace& operators;
    std::vector<OperatorInfo> operator_infos;

    GeneratorPtr construct_fork(std::vector<GeneratorPtr> nodes) const;
    GeneratorPtr construct_leaf(OperatorRange range) const;
    GeneratorPtr construct_switch(
        int switch_var_id,
        ValuesAndGenerators values_and_generators) const;
    GeneratorPtr construct_recursive(int depth, OperatorRange range) const;

public:
    explicit SuccessorGeneratorFactory(
        const VariableSpace& variables,
        const OperatorSpace& operators);
    // Destructor cannot be implicit because OperatorInfo is forward-declared.
    ~SuccessorGeneratorFactory();
    GeneratorPtr create();
};
} // namespace downward::successor_generator

#endif
