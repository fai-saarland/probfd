#include "additive_ecpdbs.h"

#include "../abstract_state.h"
#include "expcost_projection.h"

#include "../../../analysis_objectives/analysis_objective.h"
#include "../../../globals.h"

namespace probabilistic {
namespace pdbs {
namespace expected_cost {

AdditiveExpectedCostPDBs::AdditiveExpectedCostPDBs(
    std::shared_ptr<ExpCostPDBCollection> database_,
    std::shared_ptr<std::vector<PatternClique>> additive_patterns_)
    : database_(std::move(database_))
    , additive_patterns_(std::move(additive_patterns_))
{
}

EvaluationResult AdditiveExpectedCostPDBs::evaluate(const GlobalState& state)
{
    value_type::value_t result = g_analysis_objective->max();

    if (!database_->empty()) {
        // Get pattern estimates
        std::vector<value_type::value_t> estimates(database_->size());
        for (std::size_t i = 0; i != database_->size(); ++i) {
            estimates[i] = (*database_)[i]->lookup(state);
        }

        // Get lowest additive subcollection value
        for (const auto& additive_collection : *additive_patterns_) {
            value_type::value_t val = value_type::zero;

            for (PatternID id : additive_collection) {
                val += estimates[id];
            }

            result = std::min(result, val);
        }
    }

    return {false, result};
}

} // namespace expected_cost
} // namespace pdbs
} // namespace probabilistic
