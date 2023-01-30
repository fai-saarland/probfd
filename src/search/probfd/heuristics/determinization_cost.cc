#include "probfd/heuristics/determinization_cost.h"

#include "evaluation_context.h"
#include "evaluation_result.h"
#include "evaluator.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace heuristics {

DeterminizationCostHeuristic::DeterminizationCostHeuristic(
    const options::Options& opts)
    : DeterminizationCostHeuristic(
          opts.get<std::shared_ptr<Evaluator>>("evaluator"))
{
}

DeterminizationCostHeuristic::DeterminizationCostHeuristic(
    std::shared_ptr<Evaluator> evaluator)
    : evaluator_(std::move(evaluator))
{
}

DeterminizationCostHeuristic::~DeterminizationCostHeuristic() = default;

EvaluationResult
DeterminizationCostHeuristic::evaluate(const State& state) const
{
    EvaluationContext context(state);
    ::EvaluationResult result = evaluator_->compute_result(context);

    return result.is_infinite()
               ? EvaluationResult(true, -value_type::inf)
               : EvaluationResult(false, -result.get_evaluator_value());
}

void DeterminizationCostHeuristic::print_statistics() const
{
    // evaluator_->print_statistics();
}

void DeterminizationCostHeuristic::add_options_to_parser(
    options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<Evaluator>>("evaluator");
}

static Plugin<GlobalStateEvaluator> _plugin(
    "det",
    options::parse<GlobalStateEvaluator, DeterminizationCostHeuristic>);

} // namespace heuristics
} // namespace probfd
