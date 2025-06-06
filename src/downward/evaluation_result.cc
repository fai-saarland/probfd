#include "downward/evaluation_result.h"

using namespace std;

namespace downward {

const int EvaluationResult::INFTY = numeric_limits<int>::max();

EvaluationResult::EvaluationResult()
    : evaluator_value(UNINITIALIZED)
{
}

bool EvaluationResult::is_uninitialized() const
{
    return evaluator_value == UNINITIALIZED;
}

bool EvaluationResult::is_infinite() const
{
    return evaluator_value == INFTY;
}

int EvaluationResult::get_evaluator_value() const
{
    return evaluator_value;
}

const vector<OperatorID>& EvaluationResult::get_preferred_operators() const
{
    return preferred_operators;
}

bool EvaluationResult::get_count_evaluation() const
{
    return count_evaluation;
}

void EvaluationResult::set_evaluator_value(int value)
{
    evaluator_value = value;
}

void EvaluationResult::set_preferred_operators(
    vector<OperatorID>&& preferred_ops)
{
    preferred_operators = std::move(preferred_ops);
}

void EvaluationResult::set_count_evaluation(bool count_eval)
{
    count_evaluation = count_eval;
}

} // namespace downward