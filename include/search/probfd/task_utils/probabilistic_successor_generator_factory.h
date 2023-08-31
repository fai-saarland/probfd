#ifndef PROBFD_TASK_UTILS_PROBABILISTIC_SUCCESSOR_GENERATOR_FACTORY_H
#define PROBFD_TASK_UTILS_PROBABILISTIC_SUCCESSOR_GENERATOR_FACTORY_H

#include <memory>
#include <vector>

class TaskBaseProxy;

namespace probfd {
namespace successor_generator {

class ProbabilisticGeneratorBase;

using GeneratorPtr = std::unique_ptr<ProbabilisticGeneratorBase>;

struct OperatorRange;
class OperatorInfo;

class ProbabilisticSuccessorGeneratorFactory {
    using ValuesAndGenerators = std::vector<std::pair<int, GeneratorPtr>>;

    const TaskBaseProxy& task_proxy;
    std::vector<OperatorInfo> operator_infos;

    GeneratorPtr construct_fork(std::vector<GeneratorPtr> nodes) const;
    GeneratorPtr construct_leaf(OperatorRange range) const;
    GeneratorPtr construct_switch(
        int switch_var_id,
        ValuesAndGenerators values_and_generators) const;
    GeneratorPtr construct_recursive(int depth, OperatorRange range) const;

public:
    explicit ProbabilisticSuccessorGeneratorFactory(
        const TaskBaseProxy& task_proxy);
    // Destructor cannot be implicit because OperatorInfo is forward-declared.
    ~ProbabilisticSuccessorGeneratorFactory();
    GeneratorPtr create();
};

} // namespace successor_generator
} // namespace probfd

#endif
