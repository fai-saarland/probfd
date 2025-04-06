#ifndef PROBFD_TASK_UTILS_PROBABILISTIC_SUCCESSOR_GENERATOR_H
#define PROBFD_TASK_UTILS_PROBABILISTIC_SUCCESSOR_GENERATOR_H

#include <memory>
#include <vector>

// Forward Declarations
namespace downward {
class OperatorID;
class State;
class PlanningTaskProxy;
}

namespace probfd {
class TaskStateSpace;
template <typename>
struct TransitionTail;
} // namespace probfd

namespace probfd::successor_generator {
class ProbabilisticGeneratorBase;
}

namespace probfd::successor_generator {
class ProbabilisticSuccessorGenerator {
    std::unique_ptr<ProbabilisticGeneratorBase> root_;

public:
    explicit ProbabilisticSuccessorGenerator(
        const downward::PlanningTaskProxy& task_proxy);
    /*
      We cannot use the default destructor (implicitly or explicitly)
      here because GeneratorBase is a forward declaration and the
      incomplete type cannot be destroyed.
    */
    ~ProbabilisticSuccessorGenerator();

    void generate_applicable_ops(
        const downward::State& state,
        std::vector<downward::OperatorID>& applicable_ops) const;

    void generate_transitions(
        const downward::State& state,
        std::vector<TransitionTail<downward::OperatorID>>& transitions,
        TaskStateSpace& task_state_space) const;
};

} // namespace probfd::successor_generator

#endif
