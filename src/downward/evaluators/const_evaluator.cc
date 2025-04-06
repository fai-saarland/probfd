#include "downward/evaluators/const_evaluator.h"

using namespace std;

namespace downward::const_evaluator {
ConstEvaluator::ConstEvaluator(
    int value,
    const string& description,
    utils::Verbosity verbosity)
    : Evaluator(false, false, false, description, verbosity)
    , value(value)
{
}

EvaluationResult ConstEvaluator::compute_result(EvaluationContext&)
{
    EvaluationResult result;
    result.set_evaluator_value(value);
    return result;
}

} // namespace const_evaluator
