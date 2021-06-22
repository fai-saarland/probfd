#include "dead_end_pruning.h"

#include "../../global_state.h"
#include "../../heuristic.h"
#include "../../option_parser.h"
#include "../../plugin.h"
#include "../analysis_objective.h"
#include "../globals.h"

namespace probabilistic {

DeadEndPruningHeuristic::DeadEndPruningHeuristic(
    value_type::value_t default_value,
    value_type::value_t dead_end_value,
    std::shared_ptr<Heuristic> pruning_function)
    : default_value_(default_value)
    , dead_end_value_(dead_end_value)
    , pruning_function_(pruning_function)
{
}

DeadEndPruningHeuristic::DeadEndPruningHeuristic(const options::Options& opts)
    : DeadEndPruningHeuristic(
        opts.get<bool>("pessimistic") ? g_analysis_objective->min()
                                      : g_analysis_objective->max(),
        g_analysis_objective->min(),
        opts.get<std::shared_ptr<Heuristic>>("heuristic"))
{
}

void
DeadEndPruningHeuristic::add_options_to_parser(options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<Heuristic>>("heuristic");
    parser.add_option<bool>("pessimistic", "", "false");
}

EvaluationResult
DeadEndPruningHeuristic::evaluate(const GlobalState& state)
{
    pruning_function_->evaluate(state);
    if (pruning_function_->is_dead_end()) {
        return EvaluationResult(true, dead_end_value_);
    }
    return EvaluationResult(false, default_value_);
}

void
DeadEndPruningHeuristic::print_statistics() const
{
    pruning_function_->print_statistics();
}

static Plugin<GlobalStateEvaluator> _plugin(
    "prune_dead_ends",
    options::parse<GlobalStateEvaluator, DeadEndPruningHeuristic>);

} // namespace probabilistic
