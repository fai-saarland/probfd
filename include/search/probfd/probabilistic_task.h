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
    : public AbstractTaskBase
    , public subscriber::SubscriberService<ProbabilisticTask> {
public:
    ProbabilisticTask() = default;
    virtual ~ProbabilisticTask() override = default;

    std::unique_ptr<AbstractTask> build_all_outcomes_determinization();

    virtual int get_operator_cost(int index) const = 0;
    virtual std::string get_operator_name(int index) const = 0;
    virtual int get_num_operators() const = 0;
    virtual int get_num_operator_preconditions(int index) const = 0;
    virtual FactPair
    get_operator_precondition(int op_index, int fact_index) const = 0;

    virtual int get_num_operator_outcomes(int op_index) const = 0;

    virtual value_type::value_t
    get_operator_outcome_probability(int op_index, int outcome_index) const = 0;

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

    /*
     * Convert an operator index from this task, C (child), into an operator
     * index from an ancestor task A (ancestor). Task A has to be an ancestor of
     * C in the sense that C is the result of a sequence of task transformations
     * on A.
     */
    virtual int
    convert_operator_index(int index, const ProbabilisticTask* ancestor_task)
        const = 0;
};

} // namespace probfd

#endif
