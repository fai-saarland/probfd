#ifndef EVALUATORS_PREF_EVALUATOR_H
#define EVALUATORS_PREF_EVALUATOR_H

#include "downward/evaluator.h"

#include <string>
#include <vector>

namespace downward::pref_evaluator {
class PrefEvaluator : public Evaluator {
public:
    PrefEvaluator(const std::string& description, utils::Verbosity verbosity);

    virtual EvaluationResult
    compute_result(EvaluationContext& eval_context) override;
    virtual void get_path_dependent_evaluators(std::set<Evaluator*>&) override
    {
    }
};
} // namespace pref_evaluator

#endif
