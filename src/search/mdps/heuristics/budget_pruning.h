#pragma once

#include "../state_evaluator.h"
#include "../types.h"

#include <memory>
#include <unordered_map>

class Heuristic;
class StateRegistry;

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {

/**
 * @brief Budget-pruning variant of \ref DeadEndPruningHeuristic.
 */
class BudgetPruningHeuristic : public GlobalStateEvaluator {
public:
    BudgetPruningHeuristic(
        value_type::value_t default_value,
        value_type::value_t dead_end_value,
        std::shared_ptr<Heuristic> pruning_function,
        bool cache_estimates);

    explicit BudgetPruningHeuristic(const options::Options& opts);

    static void add_options_to_parser(options::OptionParser& parser);

    virtual void print_statistics() const override;

    struct Hash {
        explicit Hash(const StateRegistry* r)
            : registry_(r)
        {
        }
        std::size_t operator()(const probabilistic::StateID& id) const;
        const StateRegistry* registry_ = nullptr;
    };

    struct Equal {
        explicit Equal(const StateRegistry* r)
            : registry_(r)
        {
        }
        bool operator()(
            const probabilistic::StateID& x,
            const probabilistic::StateID& y) const;
        const StateRegistry* registry_ = nullptr;
    };

protected:
    virtual EvaluationResult evaluate(const GlobalState& state) const override;

    const value_type::value_t default_value_;
    const value_type::value_t dead_end_value_;
    std::shared_ptr<Heuristic> pruning_function_;

    const bool cache_estimates_;
    mutable std::unique_ptr<
        std::unordered_map<probabilistic::StateID, int, Hash, Equal>>
        cached_estimates_ = nullptr;
};

} // namespace probabilistic
