#include "downward/evaluators/max_evaluator.h"

#include <cassert>

using namespace std;

namespace downward::max_evaluator {
MaxEvaluator::MaxEvaluator(
    const vector<shared_ptr<Evaluator>>& evals,
    const string& description,
    utils::Verbosity verbosity)
    : CombiningEvaluator(evals, description, verbosity)
{
}

int MaxEvaluator::combine_values(const vector<int>& values)
{
    int result = 0;
    for (int value : values) {
        assert(value >= 0);
        result = max(result, value);
    }
    return result;
}

} // namespace max_evaluator
