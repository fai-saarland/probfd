#ifndef PROBFD_TASKS_DETERMINIZATION_TASK_H
#define PROBFD_TASKS_DETERMINIZATION_TASK_H

#include "probfd/aliases.h"

#include "downward/classical_operator_space.h"
#include "downward/operator_cost_function.h"
#include "probfd/probabilistic_task.h"

#include <string>
#include <utility>
#include <vector>

// Forward Declarations
namespace probfd {
class ProbabilisticOperatorSpace;
}

namespace probfd::tasks {

class DeterminizationOperatorMapping {
    std::vector<std::pair<int, int>> det_to_prob_index_;

public:
    explicit DeterminizationOperatorMapping(
        const ProbabilisticOperatorSpace& parent_operators);

    std::pair<int, int>
    get_parent_indices(int deterministic_operator_index) const;

    std::size_t num_operators() const;
};

/**
 * @brief Deterministic operator space representing the all-outcomes
 * determinization of a probabilistic operator space.
 *
 * In the all-outcomes determinization, each probabilistic operator is replaced
 * by deterministic operators, one for each probabilistic outcome, with the same
 * precondition as the original operator and the same effect as the outcome
 * that induces the deterministic operator. Essentially, every probabilistic
 * outcome can be chosen at will.
 */
class DeterminizationOperatorSpace final
    : public downward::ClassicalOperatorSpace {
    std::shared_ptr<ProbabilisticOperatorSpace> parent_operators_;
    std::shared_ptr<DeterminizationOperatorMapping> det_to_prob_index_;

public:
    /// Constructs the all-outcomes determinization of the input probabilistic
    /// planning task.
    explicit DeterminizationOperatorSpace(
        std::shared_ptr<ProbabilisticOperatorSpace> parent_task,
        std::shared_ptr<DeterminizationOperatorMapping> det_to_prob_index);

    ~DeterminizationOperatorSpace() override = default;

    std::string get_operator_name(int index) const override;

    int get_num_operators() const override;

    int get_num_operator_preconditions(int index) const override;

    downward::FactPair
    get_operator_precondition(int op_index, int fact_index) const override;

    int get_num_operator_effects(int op_index) const override;

    int get_num_operator_effect_conditions(int op_index, int eff_index)
        const override;

    downward::FactPair
    get_operator_effect_condition(int op_index, int eff_index, int cond_index)
        const override;

    downward::FactPair
    get_operator_effect(int op_index, int eff_index) const override;
};

class DeterminizationCostFunction final
    : public downward::OperatorIntCostFunction {
    std::shared_ptr<OperatorCostFunction<value_t>> parent_cost_function_;
    std::shared_ptr<DeterminizationOperatorMapping> det_to_prob_index_;

public:
    /// Constructs the all-outcomes determinization of the input probabilistic
    /// planning task.
    explicit DeterminizationCostFunction(
        std::shared_ptr<OperatorCostFunction<value_t>> parent_cost_function,
        std::shared_ptr<DeterminizationOperatorMapping> det_to_prob_index);

    ~DeterminizationCostFunction() override = default;

    int get_operator_cost(int index) const override;
};

extern downward::SharedAbstractTask
create_determinization_task(SharedProbabilisticTask probabilistic_task);

} // namespace probfd::tasks

#endif