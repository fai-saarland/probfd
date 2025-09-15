#ifndef TASK_INFORMATION_CACHE_H
#define TASK_INFORMATION_CACHE_H

#include "downward/algorithms/int_packer.h"

#include "downward/task_utils/successor_generator.h"

#include "downward/axiom_evaluator.h"
#include "downward/per_task_information.h"

namespace downward {

class TaskInformationCache {
    PerComponentInformation<AxiomEvaluator, VariableSpace, AxiomSpace>
        axiom_evaluators;
    PerComponentInformation<
        successor_generator::SuccessorGenerator,
        VariableSpace,
        OperatorSpace>
        successor_generators;
    PerComponentInformation<int_packer::IntPacker, VariableSpace>
        g_state_packers;

public:
    const AxiomEvaluator& get_axiom_evaluator(
        const VariableSpace& variables,
        const AxiomSpace& axioms);

    const successor_generator::SuccessorGenerator& get_successor_generator(
        const VariableSpace& variables,
        const OperatorSpace& operators);

    const int_packer::IntPacker& get_state_packer(
        const VariableSpace& variables);
};

} // namespace downward

#endif
