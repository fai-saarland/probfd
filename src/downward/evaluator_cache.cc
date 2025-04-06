#include "downward/evaluator_cache.h"

using namespace std;

namespace downward {

EvaluationResult& EvaluatorCache::operator[](Evaluator* eval)
{
    return eval_results[eval];
}

} // namespace downward