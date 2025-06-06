#ifndef EVALUATORS_SUM_EVALUATOR_H
#define EVALUATORS_SUM_EVALUATOR_H

#include "downward/evaluators/combining_evaluator.h"

#include <memory>
#include <vector>

namespace downward::sum_evaluator {
class SumEvaluator : public combining_evaluator::CombiningEvaluator {
protected:
    virtual int combine_values(const std::vector<int>& values) override;

public:
    SumEvaluator(
        const std::vector<std::shared_ptr<Evaluator>>& evals,
        const std::string& description,
        utils::Verbosity verbosity);
};
} // namespace sum_evaluator

#endif
