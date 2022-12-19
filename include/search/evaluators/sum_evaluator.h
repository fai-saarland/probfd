#ifndef SUM_EVALUATOR_H
#define SUM_EVALUATOR_H

#include "combining_evaluator.h"

#include <vector>
#include <memory>

namespace options {
class Options;
}

class SumEvaluator : public CombiningEvaluator {
protected:
    virtual int combine_values(const std::vector<int> &values);
public:
    SumEvaluator(const options::Options &opts);
    SumEvaluator(const std::vector<std::shared_ptr<Evaluator>> &evals);
    ~SumEvaluator();
};

#endif
