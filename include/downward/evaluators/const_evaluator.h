#ifndef EVALUATORS_CONST_EVALUATOR_H
#define EVALUATORS_CONST_EVALUATOR_H

#include "downward/evaluator.h"

namespace downward::const_evaluator {
class ConstEvaluator : public Evaluator {
    int value;

protected:
    virtual EvaluationResult
    compute_result(EvaluationContext& eval_context) override;

public:
    ConstEvaluator(
        int value,
        const std::string& description,
        utils::Verbosity verbosity);
    virtual void get_path_dependent_evaluators(std::set<Evaluator*>&) override
    {
    }
};
} // namespace const_evaluator

#endif
