#ifndef PROBFD_PROBABILISTIC_TASK_H
#define PROBFD_PROBABILISTIC_TASK_H

#include "downward/abstract_task.h" // IWYU pragma: export

#include "probfd/value_type.h"

namespace probfd {

/**
 * @brief Represents a probabilistic planning task with axioms and conditional
 * effects.
 *
 * This class should not be used directly to access the information of a
 * probabilistic planning task. To this end, use ProbabilisticTaskProxy.
 *
 * @see ProbabilisticTaskProxy
 */
class ProbabilisticTask : public AbstractTaskBase {
public:
    virtual ~ProbabilisticTask() override = default;

    /// Get the cost of the probabilistic operator with index \p op_index.
    virtual value_t get_operator_cost(int op_index) const = 0;

    /// Get the number of probabilistic outcomes of the probabilistic operator
    /// with index \p op_index.
    virtual int get_num_operator_outcomes(int op_index) const = 0;

    /// Get the probabilistic outcome probability of the outcome with index
    /// \p outcome_index of the probabilistic operator with index \p op_index.
    virtual value_t
    get_operator_outcome_probability(int op_index, int outcome_index) const = 0;

    /// Get the global outcome index for the outcome with index \p outcome_index
    /// of the probabilistic operator with index \p op_index.
    virtual int
    get_operator_outcome_id(int op_index, int outcome_index) const = 0;

    /// Get the number of effects of the outcome with index \p outcome_index of
    /// the probabilistic operator with index \p op_index.
    virtual int
    get_num_operator_outcome_effects(int op_index, int outcome_index) const = 0;

    /// Get the effect with index \p eff_index of the outcome with index
    /// \p outcome_index of the probabilistic operator with index \p op_index.
    virtual FactPair
    get_operator_outcome_effect(int op_index, int outcome_index, int eff_index)
        const = 0;

    /// Get the number of effect conditions for the effect with index
    /// \p eff_index of the outcome with index \p outcome_index of the
    /// probabilistic operator with index \p op_index.
    virtual int get_num_operator_outcome_effect_conditions(
        int op_index,
        int outcome_index,
        int eff_index) const = 0;

    /// Get the number of effect conditions for the effects with given index of
    /// outcome with given index of the probabilistic operator with given index.
    virtual FactPair get_operator_outcome_effect_condition(
        int op_index,
        int outcome_index,
        int eff_index,
        int cond_index) const = 0;
};

} // namespace probfd

#endif
