#include "budget_pruning.h"

#include "../../global_state.h"
#include "../../heuristic.h"
#include "../../option_parser.h"
#include "../../plugin.h"
#include "../analysis_objective.h"
#include "../globals.h"

namespace probabilistic {
namespace budget_pruning {

BudgetPruningHeuristic::BudgetPruningHeuristic(
    value_type::value_t default_value,
    value_type::value_t dead_end_value,
    std::shared_ptr<Heuristic> pruning_function)
    : default_value_(default_value)
    , dead_end_value_(dead_end_value)
    , pruning_function_(pruning_function)
{
}

BudgetPruningHeuristic::BudgetPruningHeuristic(const options::Options& opts)
    : BudgetPruningHeuristic(
        opts.get<bool>("pessimistic") ? g_analysis_objective->min()
                                      : g_analysis_objective->max(),
        g_analysis_objective->min(),
        opts.get<std::shared_ptr<Heuristic>>("heuristic"))
{
}

void
BudgetPruningHeuristic::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<Heuristic>>("heuristic");
    parser.add_option<bool>("pessimistic", "", "false");
}

EvaluationResult
BudgetPruningHeuristic::evaluate(const GlobalState& state)
{
    pruning_function_->evaluate(state);
    if (pruning_function_->is_dead_end()
        || (g_steps_bounded
            && pruning_function_->get_value() > state[g_step_var])) {
        return EvaluationResult(true, dead_end_value_);
    }
    return EvaluationResult(false, default_value_);
}

static Plugin<GlobalStateEvaluator> _plugin(
    "prune_budget",
    options::parse<GlobalStateEvaluator, BudgetPruningHeuristic>);

} // namespace budget_pruning
} // namespace probabilistic
