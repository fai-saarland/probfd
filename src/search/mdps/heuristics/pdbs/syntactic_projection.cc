#include "syntactic_projection.h"

namespace probabilistic {
namespace pdbs {
namespace syntactic_projection {

Outcome projectOutcome(
    const ::pdbs::Pattern& pattern,
    const ProbabilisticOutcome& outcome)
{
    Outcome projected_outcome;

    for (const GlobalEffect& effect : outcome.op->get_effects()) {
        if (std::find(pattern.begin(), pattern.end(), effect.var) != pattern.end()) {
            projected_outcome.push_back(effect);
        }
    }

    return projected_outcome;
}

SyntacticProjection buildSyntacticProjection(
    const ::pdbs::Pattern& pattern,
    const ProbabilisticOperator& op)
{
    SyntacticProjection syntactic_projection;

    for (const ProbabilisticOutcome& outcome : op) {
        const value_type::value_t probability = outcome.prob;

        Outcome projected_outcome = projectOutcome(pattern, outcome);

        auto it = syntactic_projection.find(projected_outcome);

        if (it != syntactic_projection.end()) {
            it->second += probability;
        }
        else {
            syntactic_projection[projected_outcome] = probability;
        }
    }

    return syntactic_projection;
}

bool isStochastic(const SyntacticProjection& sp_op) {
    return sp_op.size() > 1;
}

}
}
}
