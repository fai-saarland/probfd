#ifndef PROBFD_TASK_UTILS_PROBABILISTIC_SUCCESSOR_GENERATOR_H
#define PROBFD_TASK_UTILS_PROBABILISTIC_SUCCESSOR_GENERATOR_H

#include <memory>
#include <vector>

// Forward Declarations
class OperatorID;
class State;
class PlanningTaskProxy;

namespace probfd {
class TaskStateSpace;
template <typename>
struct Transition;
} // namespace probfd

namespace probfd::successor_generator {
class ProbabilisticGeneratorBase;
}

namespace probfd::successor_generator {
class ProbabilisticSuccessorGenerator {
    std::unique_ptr<ProbabilisticGeneratorBase> root_;

public:
    explicit ProbabilisticSuccessorGenerator(
        const PlanningTaskProxy& task_proxy);
    /*
      We cannot use the default destructor (implicitly or explicitly)
      here because GeneratorBase is a forward declaration and the
      incomplete type cannot be destroyed.
    */
    ~ProbabilisticSuccessorGenerator();

    void generate_applicable_ops(
        const State& state,
        std::vector<OperatorID>& applicable_ops) const;

    void generate_transitions(
        const State& state,
        std::vector<Transition<OperatorID>>& transitions,
        TaskStateSpace& task_state_space) const;
};

} // namespace probfd::successor_generator

#endif
