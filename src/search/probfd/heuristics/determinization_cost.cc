#include "probfd/heuristics/determinization_cost.h"

#include "heuristic.h"
#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace heuristics {

DeterminizationCostHeuristic::DeterminizationCostHeuristic(
    const options::Options& opts)
    : DeterminizationCostHeuristic(
          opts.get<std::shared_ptr<Heuristic>>("heuristic"))
{
}

DeterminizationCostHeuristic::DeterminizationCostHeuristic(
    std::shared_ptr<Heuristic> heuristic)
    : heuristic_(std::move(heuristic))
{
}

DeterminizationCostHeuristic::~DeterminizationCostHeuristic() = default;

EvaluationResult
DeterminizationCostHeuristic::evaluate(const GlobalState& state) const
{
    heuristic_->evaluate(state);

    return heuristic_->is_dead_end() ? EvaluationResult(true, -value_type::inf)
                                     : EvaluationResult(
                                           false,
                                           static_cast<value_type::value_t>(
                                               -heuristic_->get_heuristic()));
}

void DeterminizationCostHeuristic::print_statistics() const
{
    heuristic_->print_statistics();
}

void DeterminizationCostHeuristic::add_options_to_parser(
    options::OptionParser& parser)
{
    parser.add_option<std::shared_ptr<Heuristic>>("heuristic");
}

static Plugin<GlobalStateEvaluator> _plugin(
    "det",
    options::parse<GlobalStateEvaluator, DeterminizationCostHeuristic>);

} // namespace heuristics
} // namespace probfd
