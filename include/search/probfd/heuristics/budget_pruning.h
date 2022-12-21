#ifndef MDPS_HEURISTICS_BUDGET_PRUNING_H
#define MDPS_HEURISTICS_BUDGET_PRUNING_H

#include "probfd/state_evaluator.h"
#include "probfd/types.h"

#include <memory>
#include <unordered_map>

namespace legacy {
class Heuristic;
class StateRegistry;
}


namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probfd {
namespace heuristics {

/**
 * @brief Budget-pruning variant of \ref DeadEndPruningHeuristic.
 */
class BudgetPruningHeuristic : public GlobalStateEvaluator {
public:
    BudgetPruningHeuristic(
        value_type::value_t default_value,
        value_type::value_t dead_end_value,
        std::shared_ptr<legacy::Heuristic> pruning_function,
        bool cache_estimates);

    explicit BudgetPruningHeuristic(const options::Options& opts);

    static void add_options_to_parser(options::OptionParser& parser);

    virtual void print_statistics() const override;

    struct Hash {
        explicit Hash(const legacy::StateRegistry* r)
            : registry_(r)
        {
        }
        std::size_t operator()(const StateID& id) const;
        const legacy::StateRegistry* registry_ = nullptr;
    };

    struct Equal {
        explicit Equal(const legacy::StateRegistry* r)
            : registry_(r)
        {
        }
        bool operator()(const StateID& x, const StateID& y) const;
        const legacy::StateRegistry* registry_ = nullptr;
    };

protected:
    virtual EvaluationResult
    evaluate(const legacy::GlobalState& state) const override;

    const value_type::value_t default_value_;
    const value_type::value_t dead_end_value_;
    std::shared_ptr<legacy::Heuristic> pruning_function_;

    const bool cache_estimates_;
    mutable std::unique_ptr<std::unordered_map<StateID, int, Hash, Equal>>
        cached_estimates_ = nullptr;
};

} // namespace heuristics
} // namespace probfd

#endif // __BUDGET_PRUNING_H__