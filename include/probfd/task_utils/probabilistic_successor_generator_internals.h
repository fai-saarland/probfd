#ifndef PROBFD_TASK_UTILS_PROBABILISTIC_SUCCESSOR_GENERATOR_INTERNALS_H
#define PROBFD_TASK_UTILS_PROBABILISTIC_SUCCESSOR_GENERATOR_INTERNALS_H

#include "downward/operator_id.h"

#include <memory>
#include <unordered_map>
#include <vector>

class State;

namespace probfd {
class TaskStateSpace;
template <typename>
struct TransitionTail;
} // namespace probfd

namespace probfd::successor_generator {

class ProbabilisticGeneratorBase {
public:
    virtual ~ProbabilisticGeneratorBase() = default;

    virtual void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<OperatorID>& applicable_ops) const = 0;

    virtual void generate_transitions(
        const State& state,
        std::vector<TransitionTail<OperatorID>>& transitions,
        TaskStateSpace& task_state_space) const = 0;
};

class ProbabilisticGeneratorForkBinary : public ProbabilisticGeneratorBase {
    std::unique_ptr<ProbabilisticGeneratorBase> generator_1_;
    std::unique_ptr<ProbabilisticGeneratorBase> generator_2_;

public:
    ProbabilisticGeneratorForkBinary(
        std::unique_ptr<ProbabilisticGeneratorBase> generator1,
        std::unique_ptr<ProbabilisticGeneratorBase> generator2);

    void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<OperatorID>& applicable_ops) const override;

    void generate_transitions(
        const State& state,
        std::vector<TransitionTail<OperatorID>>& transitions,
        TaskStateSpace& task_state_space) const override;
};

class ProbabilisticGeneratorForkMulti : public ProbabilisticGeneratorBase {
    std::vector<std::unique_ptr<ProbabilisticGeneratorBase>> children_;

public:
    explicit ProbabilisticGeneratorForkMulti(
        std::vector<std::unique_ptr<ProbabilisticGeneratorBase>> children);

    void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<OperatorID>& applicable_ops) const override;

    void generate_transitions(
        const State& state,
        std::vector<TransitionTail<OperatorID>>& transitions,
        TaskStateSpace& task_state_space) const override;
};

class ProbabilisticGeneratorSwitchVector : public ProbabilisticGeneratorBase {
    int switch_var_id_;
    std::vector<std::unique_ptr<ProbabilisticGeneratorBase>>
        generator_for_value_;

public:
    ProbabilisticGeneratorSwitchVector(
        int switch_var_id,
        std::vector<std::unique_ptr<ProbabilisticGeneratorBase>>&&
            generator_for_value);

    void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<OperatorID>& applicable_ops) const override;

    void generate_transitions(
        const State& state,
        std::vector<TransitionTail<OperatorID>>& transitions,
        TaskStateSpace& task_state_space) const override;
};

class ProbabilisticGeneratorSwitchHash : public ProbabilisticGeneratorBase {
    int switch_var_id_;
    std::unordered_map<int, std::unique_ptr<ProbabilisticGeneratorBase>>
        generator_for_value_;

public:
    ProbabilisticGeneratorSwitchHash(
        int switch_var_id,
        std::unordered_map<int, std::unique_ptr<ProbabilisticGeneratorBase>>&&
            generator_for_value);

    void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<OperatorID>& applicable_ops) const override;

    void generate_transitions(
        const State& state,
        std::vector<TransitionTail<OperatorID>>& transitions,
        TaskStateSpace& task_state_space) const override;
};

class ProbabilisticGeneratorSwitchSingle : public ProbabilisticGeneratorBase {
    int switch_var_id_;
    int value_;
    std::unique_ptr<ProbabilisticGeneratorBase> generator_for_value_;

public:
    ProbabilisticGeneratorSwitchSingle(
        int switch_var_id,
        int value,
        std::unique_ptr<ProbabilisticGeneratorBase> generator_for_value);

    void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<OperatorID>& applicable_ops) const override;

    void generate_transitions(
        const State& state,
        std::vector<TransitionTail<OperatorID>>& transitions,
        TaskStateSpace& task_state_space) const override;
};

class ProbabilisticGeneratorLeafVector : public ProbabilisticGeneratorBase {
    std::vector<OperatorID> applicable_operators_;

public:
    explicit ProbabilisticGeneratorLeafVector(
        std::vector<OperatorID>&& applicable_operators);

    void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<OperatorID>& applicable_ops) const override;

    void generate_transitions(
        const State& state,
        std::vector<TransitionTail<OperatorID>>& transitions,
        TaskStateSpace& task_state_space) const override;
};

class ProbabilisticGeneratorLeafSingle : public ProbabilisticGeneratorBase {
    OperatorID applicable_operator_;

public:
    explicit ProbabilisticGeneratorLeafSingle(OperatorID applicable_operator);

    void generate_applicable_ops(
        const std::vector<int>& state,
        std::vector<OperatorID>& applicable_ops) const override;

    void generate_transitions(
        const State& state,
        std::vector<TransitionTail<OperatorID>>& transitions,
        TaskStateSpace& task_state_space) const override;
};

} // namespace probfd::successor_generator

#endif
