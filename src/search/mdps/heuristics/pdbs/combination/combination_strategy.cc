#include "combination_strategy.h"

#include "../../../analysis_objectives/analysis_objective.h"
#include "../../../globals.h"

#include "../../../../plugin.h"

#include "../expected_cost/expcost_projection.h"
#include "../maxprob/maxprob_projection.h"

namespace probabilistic {
namespace pdbs {

template <class PDBType>
EvaluationResult CombinationStrategy::evaluate(
    const PPDBCollection<PDBType>& database,
    const std::vector<PatternSubCollection>& subcollections,
    const GlobalState& state) const
{
    value_type::value_t result = g_analysis_objective->max();

    if (!database.empty()) {
        // Get pattern estimates
        std::vector<value_type::value_t> estimates(database.size());
        for (std::size_t i = 0; i != database.size(); ++i) {
            auto eval_result = database[i]->evaluate(state);

            if (eval_result) {
                return eval_result;
            }

            estimates[i] = static_cast<value_type::value_t>(eval_result);
        }

        // Get lowest additive subcollection value
        for (const auto& subcollection : subcollections) {
            const auto val = this->combine(estimates, subcollection);
            result = std::min(result, val);
        }
    }

    return {false, result};
}

template EvaluationResult CombinationStrategy::evaluate(
    const PPDBCollection<expected_cost::ExpCostProjection>& database,
    const std::vector<PatternSubCollection>& subcollections,
    const GlobalState& state) const;

template EvaluationResult CombinationStrategy::evaluate(
    const PPDBCollection<maxprob::MaxProbProjection>& database,
    const std::vector<PatternSubCollection>& subcollections,
    const GlobalState& state) const;

static PluginTypePlugin<CombinationStrategy> _plugin("CombinationStrategy", "");

} // namespace pdbs
} // namespace probabilistic
