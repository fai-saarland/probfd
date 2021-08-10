#include "multiplicative_mppdbs.h"

#include "../abstract_state.h"
#include "maxprob_projection.h"

#include "../../../analysis_objectives/analysis_objective.h"
#include "../../../globals.h"

namespace probabilistic {
namespace pdbs {
namespace maxprob {

MultiplicativeMaxProbPDBs::MultiplicativeMaxProbPDBs(
    std::shared_ptr<MaxProbPDBCollection> database_,
    std::shared_ptr<std::vector<PatternClique>> multiplicative_patterns_)
    : database_(std::move(database_))
    , multiplicative_patterns_(std::move(multiplicative_patterns_))
{
}

EvaluationResult
MultiplicativeMaxProbPDBs::evaluate(const GlobalState& state) const
{
    value_type::value_t result = g_analysis_objective->max();

    if (!database_->empty()) {
        // Get pattern estimates
        std::vector<value_type::value_t> estimates(database_->size());
        for (std::size_t i = 0; i != database_->size(); ++i) {
            auto eval_result = (*database_)[i]->evaluate(state);

            if (eval_result) {
                return eval_result;
            }

            estimates[i] = static_cast<value_type::value_t>(eval_result);
        }

        // Get lowest additive subcollection value
        for (const auto& subcollection : *multiplicative_patterns_) {
            value_type::value_t val = value_type::one;

            for (PatternID id : subcollection) {
                val *= estimates[id];
            }

            result = std::min(result, val);
        }
    }

    return {false, result};
}

} // namespace maxprob
} // namespace pdbs
} // namespace probabilistic
