#include "downward/heuristic.h"

#include "downward/evaluation_context.h"
#include "downward/evaluation_result.h"
#include "downward/task_transformation.h"

#include "downward/transformations/identity_transformation.h"

#include "downward/task_utils/task_properties.h"
#include "downward/tasks/root_task.h"

#include <cassert>

using namespace std;

namespace downward {

Heuristic::Heuristic(
    std::shared_ptr<AbstractTask> original_task,
    std::shared_ptr<AbstractTask> transformed_task,
    std::shared_ptr<StateMapping> state_mapping,
    std::shared_ptr<InverseOperatorMapping> inv_operator_mapping,
    bool cache_estimates,
    const std::string& description,
    utils::Verbosity verbosity)
    : Evaluator(true, true, true, description, verbosity)
    , heuristic_cache(HEntry(NO_VALUE, true))
    // TODO: is true really a good idea here?
    , cache_evaluator_values(cache_estimates)
    , original_task(std::move(original_task))
    , transformed_task(std::move(transformed_task))
    , state_mapping(std::move(state_mapping))
    , inv_operator_mapping(std::move(inv_operator_mapping))
    , task_proxy(*this->transformed_task)
{
}

Heuristic::Heuristic(
    std::shared_ptr<AbstractTask> original_task,
    TaskTransformationResult transformation_result,
    bool cache_estimates,
    const std::string& description,
    utils::Verbosity verbosity)
    : Heuristic(
          std::move(original_task),
          std::move(transformation_result.transformed_task),
          std::move(transformation_result.state_mapping),
          std::move(transformation_result.inv_operator_mapping),
          cache_estimates,
          description,
          verbosity)

{
}

Heuristic::Heuristic(
    std::shared_ptr<AbstractTask> original_task,
    const std::shared_ptr<TaskTransformation> transformation,
    bool cache_estimates,
    const std::string& description,
    utils::Verbosity verbosity)
    : Heuristic(
          original_task,
          transformation->transform(original_task),
          cache_estimates,
          description,
          verbosity)

{
}

Heuristic::~Heuristic() = default;

void Heuristic::set_preferred(const OperatorProxy& op)
{
    preferred_operators.insert(
        inv_operator_mapping->get_ancestor_operator_id(op.get_id()));
}

State Heuristic::convert_ancestor_state(const State& ancestor_state) const
{
    return state_mapping->convert_ancestor_state(
        ancestor_state,
        *transformed_task);
}

EvaluationResult Heuristic::compute_result(EvaluationContext& eval_context)
{
    EvaluationResult result;

    assert(preferred_operators.empty());

    const State& state = eval_context.get_state();
    bool calculate_preferred = eval_context.get_calculate_preferred();

    int heuristic = NO_VALUE;

    if (!calculate_preferred && cache_evaluator_values &&
        heuristic_cache[state].h != NO_VALUE && !heuristic_cache[state].dirty) {
        heuristic = heuristic_cache[state].h;
        result.set_count_evaluation(false);
    } else {
        heuristic = compute_heuristic(state);
        if (cache_evaluator_values) {
            heuristic_cache[state] = HEntry(heuristic, false);
        }
        result.set_count_evaluation(true);
    }

    assert(heuristic == DEAD_END || heuristic >= 0);

    if (heuristic == DEAD_END) {
        /*
          It is permissible to mark preferred operators for dead-end
          states (thus allowing a heuristic to mark them on-the-fly
          before knowing the final result), but if it turns out we
          have a dead end, we don't want to actually report any
          preferred operators.
        */
        preferred_operators.clear();
        heuristic = EvaluationResult::INFTY;
    }

#ifndef NDEBUG
    PlanningTaskProxy global_task_proxy(*original_task);
    PartialOperatorsProxy global_operators =
        global_task_proxy.get_partial_operators();
    if (heuristic != EvaluationResult::INFTY) {
        for (OperatorID op_id : preferred_operators)
            assert(
                task_properties::is_applicable(global_operators[op_id], state));
    }
#endif

    result.set_evaluator_value(heuristic);
    result.set_preferred_operators(preferred_operators.pop_as_vector());
    assert(preferred_operators.empty());

    return result;
}

bool Heuristic::does_cache_estimates() const
{
    return cache_evaluator_values;
}

bool Heuristic::is_estimate_cached(const State& state) const
{
    return heuristic_cache[state].h != NO_VALUE;
}

int Heuristic::get_cached_estimate(const State& state) const
{
    assert(is_estimate_cached(state));
    return heuristic_cache[state].h;
}

} // namespace downward