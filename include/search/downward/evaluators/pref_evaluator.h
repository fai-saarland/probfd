#ifndef EVALUATORS_PREF_EVALUATOR_H
#define EVALUATORS_PREF_EVALUATOR_H

#include "downward/evaluator.h"

#include <string>
#include <vector>

namespace pref_evaluator {
class PrefEvaluator : public Evaluator {
public:
    explicit PrefEvaluator(const plugins::Options& opts);
    virtual ~PrefEvaluator() override;

    virtual EvaluationResult
    compute_result(EvaluationContext& eval_context) override;
    virtual void get_path_dependent_evaluators(std::set<Evaluator*>&) override
    {
    }
};
} // namespace pref_evaluator

#endif
