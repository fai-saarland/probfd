#pragma once

#include <map>
#include <vector>

#include "../../globals.h"
#include "../../../global_operator.h"
#include "types.h"


namespace probabilistic {
namespace pdbs {
namespace syntactic_projection {

using Outcome = std::vector<GlobalEffect>;

struct outcome_less {
    static bool eff_less(const GlobalEffect& eff1, const GlobalEffect& eff2) {
        return std::tie(eff1.var, eff1.val) < std::tie(eff2.var, eff2.val);
    }

    bool operator()(const Outcome& a, const Outcome& b) const {
        return std::lexicographical_compare(
            a.begin(), a.end(), b.begin(), b.end(), eff_less);
    }
};

using SyntacticProjection = std::map<Outcome, value_type::value_t, outcome_less>;

Outcome project_outcome(
    const Pattern& pattern,
    const ProbabilisticOutcome& outcome);

SyntacticProjection build_syntactic_projection(
    const Pattern& pattern,
    const ProbabilisticOperator& op);

bool is_stochastic(const SyntacticProjection& sp);

}
}
}
