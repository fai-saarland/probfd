#pragma once

#include "../state_evaluator.h"

#include <memory>

class Heuristic;

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {
namespace dead_end_pruning {

class DeadEndPruningHeuristic : public GlobalStateEvaluator {
public:
    DeadEndPruningHeuristic(
        value_type::value_t default_value,
        value_type::value_t dead_end_value,
        std::shared_ptr<Heuristic> pruning_function);

    explicit DeadEndPruningHeuristic(const options::Options& opts);

    static void add_options_to_parser(options::OptionParser& parser);

    virtual void print_statistics() const override;

protected:
    virtual EvaluationResult evaluate(const GlobalState& state) override;

    const value_type::value_t default_value_;
    const value_type::value_t dead_end_value_;
    std::shared_ptr<Heuristic> pruning_function_;
};

} // namespace dead_end_pruning
} // namespace probabilistic
