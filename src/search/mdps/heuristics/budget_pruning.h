#pragma once

#include "../state_evaluator.h"

#include <memory>

class Heuristic;

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {
namespace budget_pruning {

class BudgetPruningHeuristic : public GlobalStateEvaluator {
public:
    BudgetPruningHeuristic(
        value_type::value_t default_value,
        value_type::value_t dead_end_value,
        std::shared_ptr<Heuristic> pruning_function);

    explicit BudgetPruningHeuristic(const options::Options& opts);

    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual EvaluationResult evaluate(const GlobalState& state) override;

    const value_type::value_t default_value_;
    const value_type::value_t dead_end_value_;
    std::shared_ptr<Heuristic> pruning_function_;
};

} // namespace budget_pruning
} // namespace probabilistic
