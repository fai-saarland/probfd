#include "downward/evaluators/pref_evaluator.h"

#include "downward/evaluation_context.h"
#include "downward/evaluation_result.h"

using namespace std;

namespace downward::pref_evaluator {
PrefEvaluator::PrefEvaluator(
    const string& description,
    utils::Verbosity verbosity)
    : Evaluator(false, false, false, description, verbosity)
{
}

EvaluationResult PrefEvaluator::compute_result(EvaluationContext& eval_context)
{
    EvaluationResult result;
    if (eval_context.is_preferred())
        result.set_evaluator_value(0);
    else
        result.set_evaluator_value(1);
    return result;
}

} // namespace pref_evaluator
