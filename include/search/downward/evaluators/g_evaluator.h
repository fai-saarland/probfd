#ifndef EVALUATORS_G_EVALUATOR_H
#define EVALUATORS_G_EVALUATOR_H

#include "downward/evaluator.h"

namespace g_evaluator {
class GEvaluator : public Evaluator {
public:
    explicit GEvaluator(const plugins::Options& opts);
    virtual ~GEvaluator() override = default;

    virtual EvaluationResult
    compute_result(EvaluationContext& eval_context) override;

    virtual void get_path_dependent_evaluators(std::set<Evaluator*>&) override
    {
    }
};
} // namespace g_evaluator

#endif
