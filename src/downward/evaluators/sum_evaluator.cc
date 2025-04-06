#include "downward/evaluators/sum_evaluator.h"

#include <cassert>
#include <limits>

using namespace std;

namespace downward::sum_evaluator {
SumEvaluator::SumEvaluator(
    const vector<shared_ptr<Evaluator>>& evals,
    const string& description,
    utils::Verbosity verbosity)
    : CombiningEvaluator(evals, description, verbosity)
{
}

int SumEvaluator::combine_values(const vector<int>& values)
{
    int result = 0;
    for (int value : values) {
        assert(value >= 0);
        result += value;
        assert(result >= 0); // Check against overflow.
    }
    return result;
}

} // namespace sum_evaluator
