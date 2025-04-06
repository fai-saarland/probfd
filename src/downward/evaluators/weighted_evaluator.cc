#include "downward/evaluators/weighted_evaluator.h"

#include "downward/evaluation_context.h"
#include "downward/evaluation_result.h"

#include <sstream>

using namespace std;

namespace downward::weighted_evaluator {

WeightedEvaluator::WeightedEvaluator(
    const shared_ptr<Evaluator>& eval,
    int weight,
    const string& description,
    utils::Verbosity verbosity)
    : Evaluator(false, false, false, description, verbosity)
    , evaluator(eval)
    , weight(weight)
{
}

bool WeightedEvaluator::dead_ends_are_reliable() const
{
    return evaluator->dead_ends_are_reliable();
}

EvaluationResult
WeightedEvaluator::compute_result(EvaluationContext& eval_context)
{
    // Note that this produces no preferred operators.
    EvaluationResult result;
    int value = eval_context.get_evaluator_value_or_infinity(evaluator.get());
    if (value != EvaluationResult::INFTY) {
        // TODO: Check for overflow?
        value *= weight;
    }
    result.set_evaluator_value(value);
    return result;
}

void WeightedEvaluator::get_path_dependent_evaluators(set<Evaluator*>& evals)
{
    evaluator->get_path_dependent_evaluators(evals);
}

} // namespace weighted_evaluator
