#pragma once

#include <map>
#include <vector>

#include "../../globals.h"
#include "../../../global_operator.h"
#include "../../../pdbs/types.h"


namespace probabilistic {
namespace pdbs {
namespace syntactic_projection {

struct cmp_outcome {
    struct cmp_eff {
        bool operator()(const GlobalEffect& eff1, const GlobalEffect& eff2) const {
            return eff1.var < eff2.var || (eff1.var == eff2.var && eff1.val < eff2.val);
        }
    };

    bool operator()(const std::vector<GlobalEffect>& a, const std::vector<GlobalEffect>& b) const {
        return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end(), cmp_eff());
    }
};

using Outcome = std::vector<GlobalEffect>;
using SyntacticProjection = std::map<Outcome, value_type::value_t, cmp_outcome>;

Outcome projectOutcome(
    const ::pdbs::Pattern& pattern,
    const ProbabilisticOutcome& outcome);

SyntacticProjection buildSyntacticProjection(
    const ::pdbs::Pattern& pattern,
    const ProbabilisticOperator& op);

bool isStochastic(const SyntacticProjection& sp_op);

}
}
}
