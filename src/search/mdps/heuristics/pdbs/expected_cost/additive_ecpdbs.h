#pragma once

#include "../../../evaluation_result.h"
#include "../types.h"

#include <memory>
#include <vector>

class GlobalState;

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {
namespace pdbs {
namespace expected_cost {

class AdditiveExpectedCostPDBs {
    std::shared_ptr<ExpCostPDBCollection> database_;
    std::shared_ptr<std::vector<PatternClique>> additive_patterns_;

public:
    explicit AdditiveExpectedCostPDBs(
        std::shared_ptr<ExpCostPDBCollection> database_,
        std::shared_ptr<std::vector<PatternClique>> additive_patterns_);

    EvaluationResult evaluate(const GlobalState& state);
};

} // namespace expected_cost
} // namespace pdbs
} // namespace probabilistic
