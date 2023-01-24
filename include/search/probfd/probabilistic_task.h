#ifndef PROBFD_PROBABILISTIC_TASK_H
#define PROBFD_PROBABILISTIC_TASK_H

#include "operator_id.h"

#include "algorithms/subscriber.h"
#include "utils/hash.h"

#include "abstract_task.h"

#include "probfd/value_type.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace options {
class Options;
}

namespace probfd {

class ProbabilisticTask
    : public AbstractTaskBase {
public:
    ProbabilisticTask() = default;
    virtual ~ProbabilisticTask() override = default;

    std::unique_ptr<AbstractTask> build_all_outcomes_determinization();

    virtual int get_num_operator_outcomes(int op_index) const = 0;

    virtual value_t
    get_operator_outcome_probability(int op_index, int outcome_index) const = 0;

    virtual int
    get_operator_outcome_id(int op_index, int outcome_index) const = 0;

    virtual int
    get_num_operator_outcome_effects(int op_index, int outcome_index) const = 0;
    virtual FactPair
    get_operator_outcome_effect(int op_index, int outcome_index, int eff_index)
        const = 0;

    virtual int get_num_operator_outcome_effect_conditions(
        int op_index,
        int outcome_index,
        int eff_index) const = 0;
    virtual FactPair get_operator_outcome_effect_condition(
        int op_index,
        int outcome_index,
        int eff_index,
        int cond_index) const = 0;

    // Convenience function returning the negative operator cost
    int get_operator_reward(int op_index) const
    {
        return -this->get_operator_cost(op_index);
    }
};

} // namespace probfd

#endif
