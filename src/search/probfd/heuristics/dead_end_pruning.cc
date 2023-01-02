#include "probfd/heuristics/dead_end_pruning.h"

#include "probfd/analysis_objectives/analysis_objective.h"

#include "utils/system.h"

#include "evaluation_context.h"
#include "evaluation_result.h"
#include "evaluator.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace heuristics {

DeadEndPruningHeuristic::DeadEndPruningHeuristic(
    value_type::value_t default_value,
    value_type::value_t dead_end_value,
    std::shared_ptr<Evaluator> pruning_function)
    : default_value_(default_value)
    , dead_end_value_(dead_end_value)
    , pruning_function_(pruning_function)
{
    if (pruning_function_->dead_ends_are_reliable()) {
        utils::g_log << "Dead end pruning heuristic was constructed with an "
                        "evaluator that has unreliable dead ends!"
                     << std::endl;
        utils::exit_with(utils::ExitCode::SEARCH_INPUT_ERROR);
    }
}

DeadEndPruningHeuristic::DeadEndPruningHeuristic(const options::Options& opts)
    : DeadEndPruningHeuristic(
          opts.get<bool>("pessimistic")
              ? g_analysis_objective->reward_bound().lower
              : g_analysis_objective->reward_bound().upper,
          g_analysis_objective->reward_bound().lower,
          opts.get<std::shared_ptr<Evaluator>>("heuristic"))
{
}

void DeadEndPruningHeuristic::add_options_to_parser(
    options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<Evaluator>>("evaluator");
    parser.add_option<bool>("pessimistic", "", "false");
}

EvaluationResult DeadEndPruningHeuristic::evaluate(const State& state) const
{
    EvaluationContext context(state);
    ::EvaluationResult result = pruning_function_->compute_result(context);

    if (result.is_infinite()) {
        return EvaluationResult(true, dead_end_value_);
    }
    return EvaluationResult(false, default_value_);
}

void DeadEndPruningHeuristic::print_statistics() const
{
    // pruning_function_->print_statistics();
}

static Plugin<GlobalStateEvaluator> _plugin(
    "prune_dead_ends",
    options::parse<GlobalStateEvaluator, DeadEndPruningHeuristic>);

} // namespace heuristics
} // namespace probfd
