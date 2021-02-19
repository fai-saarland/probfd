#include "syntactic_projection.h"

namespace probabilistic {
namespace pdbs {
namespace syntactic_projection {

Outcome project_outcome(
    const Pattern& pattern,
    const ProbabilisticOutcome& outcome)
{
    Outcome projected_outcome;

    for (const GlobalEffect& effect : outcome.op->get_effects()) {
        const auto it = std::find(pattern.begin(), pattern.end(), effect.var);
        if (it != pattern.end()) {
            projected_outcome.push_back(effect);
        }
    }

    return projected_outcome;
}

SyntacticProjection build_syntactic_projection(
    const Pattern& pattern,
    const ProbabilisticOperator& op)
{
    SyntacticProjection syntactic_projection;

    for (const ProbabilisticOutcome& outcome : op) {
        const value_type::value_t probability = outcome.prob;

        Outcome projected_outcome = project_outcome(pattern, outcome);

        auto it = syntactic_projection.find(projected_outcome);

        if (it != syntactic_projection.end()) {
            it->second += probability;
        } else {
            syntactic_projection[projected_outcome] = probability;
        }
    }

    return syntactic_projection;
}

bool is_stochastic(const SyntacticProjection& sp) {
    return sp.size() > 1;
}

}
}
}
