#ifndef WEIGHTED_EVALUATOR_H
#define WEIGHTED_EVALUATOR_H

#include "evaluator.h"

#include <string>
#include <vector>
#include <memory>

namespace options {
class Options;
}

class WeightedEvaluator : public Evaluator {
private:
    std::shared_ptr<Evaluator> evaluator;
    int w;
    int value;

public:
    WeightedEvaluator(const options::Options &opts);
    WeightedEvaluator(std::shared_ptr<Evaluator> eval, int weight);
    ~WeightedEvaluator();

    void evaluate(int g, bool preferred);
    bool is_dead_end() const;
    bool dead_end_is_reliable() const;
    int get_value() const;
    void get_involved_heuristics(std::set<Heuristic *> &hset);
};

#endif
