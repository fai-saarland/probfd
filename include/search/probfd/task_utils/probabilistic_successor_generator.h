#ifndef PROBFD_TASK_UTILS_PROBABILISTIC_SUCCESSOR_GENERATOR_H
#define PROBFD_TASK_UTILS_PROBABILISTIC_SUCCESSOR_GENERATOR_H

#include <memory>
#include <vector>

class OperatorID;
class State;
class TaskBaseProxy;

namespace probfd {
class TaskStateSpace;

template <typename>
struct Transition;

namespace successor_generator {
class ProbabilisticGeneratorBase;

class ProbabilisticSuccessorGenerator {
    std::unique_ptr<ProbabilisticGeneratorBase> root;

public:
    explicit ProbabilisticSuccessorGenerator(const TaskBaseProxy& task_proxy);
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

} // namespace successor_generator
} // namespace probfd

#endif
