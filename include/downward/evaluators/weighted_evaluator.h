#ifndef EVALUATORS_WEIGHTED_EVALUATOR_H
#define EVALUATORS_WEIGHTED_EVALUATOR_H

#include "downward/evaluator.h"

#include <memory>

namespace downward::weighted_evaluator {
class WeightedEvaluator : public Evaluator {
    std::shared_ptr<Evaluator> evaluator;
    int weight;

public:
    WeightedEvaluator(
        const std::shared_ptr<Evaluator>& eval,
        int weight,
        const std::string& description,
        utils::Verbosity verbosity);

    virtual bool dead_ends_are_reliable() const override;
    virtual EvaluationResult
    compute_result(EvaluationContext& eval_context) override;
    virtual void
    get_path_dependent_evaluators(std::set<Evaluator*>& evals) override;
};
} // namespace weighted_evaluator

#endif
