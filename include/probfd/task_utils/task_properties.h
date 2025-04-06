#ifndef PROBFD_TASK_UTILS_TASK_PROPERTIES_H
#define PROBFD_TASK_UTILS_TASK_PROPERTIES_H

#include "probfd/task_proxy.h"

#include "probfd/value_type.h"

#include "downward/operator_cost.h"
#include "downward/task_proxy.h"

#include <iosfwd>
#include <iterator>
#include <vector>

// Forward Declarations
namespace downward::utils {
class LogProxy;
}

namespace probfd {
class ProbabilisticTask;
}

namespace probfd::task_properties {

/**
 * @brief Outputs the affected variables of this probabilistic operator.
 *
 * Each variable is output as many times as there are effects of the operator
 * mentioning this variable.
 * The order in which affected variables are output is unspecified.
 */
void get_affected_vars(
    const ProbabilisticOperatorProxy& op,
    std::output_iterator<int> auto it)
{
    for (const ProbabilisticOutcomeProxy& outcome : op.get_outcomes()) {
        for (const auto& effect : outcome.get_effects()) {
            *it = effect.get_fact().get_variable().get_id();
        }
    }
}

/**
 * @brief Checks if a probabilistic operator is applicable in a state.
 */
extern bool is_applicable(
    const ProbabilisticOperatorProxy& op,
    const downward::State& state);

extern value_t get_adjusted_action_cost(
    const ProbabilisticOperatorProxy& op,
    downward::OperatorCost cost_type,
    bool is_unit_cost);

/**
 * @brief Checks if all operators have cost 1.0 (exact fp equality).
 *
 * Runtime: O(n), where n is the number of probabilistic operators.
 */
extern bool is_unit_cost(const ProbabilisticTaskProxy& task);

/**
 * @brief Checks if conditional effects exist.
 *
 * Runtime: O(n), where n is the number of effects.
 */
extern bool has_conditional_effects(const ProbabilisticTaskProxy& task);

/**
 * @brief Throws ExitCode::UNSUPPORTED if conditional effects exist.
 *
 * Runtime: O(n), where n is the number of effects.
 */
extern void verify_no_conditional_effects(const ProbabilisticTaskProxy& task);

/**
 * @brief Returns the operator costs of a task as a vector of costs sorted by
 * operator id.
 *
 * Runtime: O(n), where n is the number of operators.
 */
extern std::vector<value_t>
get_operator_costs(const ProbabilisticTaskProxy& task_proxy);

/**
 * @brief Returns the average operator cost of a task among all operators.
 *
 * Runtime: O(n), where n is the number of operators.
 */
extern value_t
get_average_operator_cost(const ProbabilisticTaskProxy& task_proxy);

/**
 * @brief Returns the minimum operator cost of a task among all operators.
 *
 * Runtime: O(n), where n is the number of operators.
 */
extern value_t get_min_operator_cost(const ProbabilisticTaskProxy& task_proxy);

/**
 * @brief Returns the minimum operator cost of an operator space among all
 * operators.
 *
 * Runtime: O(n), where n is the number of operators.
 */
extern value_t get_min_operator_cost(const ProbabilisticOperatorsProxy& ops);

/**
 * @brief Return the total number of effects of the task, including the
 * effects of axioms.
 *
 * Runtime: O(m + n), where m is the number of axioms and n is the total
 * number of operator effects.
 */
extern int get_num_total_effects(const ProbabilisticTaskProxy& task_proxy);

/**
 * @brief Dumps a probabilistic task to a given log.
 */
extern void dump_probabilistic_task(
    const ProbabilisticTaskProxy& task_proxy,
    downward::utils::LogProxy& log);

/**
 * @brief Dumps a probabilistic task to a given output stream.
 */
extern void dump_probabilistic_task(
    const ProbabilisticTaskProxy& task_proxy,
    std::ostream& os);

} // namespace probfd::task_properties

#endif
