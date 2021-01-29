#pragma once

#include "../state_evaluator.h"

#include <memory>

class Heuristic;

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic::determinization_heuristic {

class DeterminizationCostHeuristic : public GlobalStateEvaluator {
    std::shared_ptr<Heuristic> heuristic_;

public:
    explicit DeterminizationCostHeuristic(const options::Options& opts);
    explicit DeterminizationCostHeuristic(std::shared_ptr<Heuristic> heuristic);

    ~DeterminizationCostHeuristic() override;

    static void add_options_to_parser(options::OptionParser& parser);

    void print_statistics() const override;

protected:
    EvaluationResult evaluate(const GlobalState &state) override;
};


} // namespace probabilistic::determinization_heuristic
