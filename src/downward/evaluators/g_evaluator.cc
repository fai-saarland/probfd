#include "downward/evaluators/g_evaluator.h"

#include "downward/evaluation_context.h"
#include "downward/evaluation_result.h"

using namespace std;

namespace downward::g_evaluator {
GEvaluator::GEvaluator(const string& description, utils::Verbosity verbosity)
    : Evaluator(false, false, false, description, verbosity)
{
}

EvaluationResult GEvaluator::compute_result(EvaluationContext& eval_context)
{
    EvaluationResult result;
    result.set_evaluator_value(eval_context.get_g_value());
    return result;
}

} // namespace g_evaluator
