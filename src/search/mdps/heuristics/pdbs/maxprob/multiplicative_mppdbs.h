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
namespace maxprob {

class MultiplicativeMaxProbPDBs {
    std::shared_ptr<MaxProbPDBCollection> database_;
    std::shared_ptr<std::vector<PatternClique>> multiplicative_patterns_;

public:
    explicit MultiplicativeMaxProbPDBs(
        std::shared_ptr<MaxProbPDBCollection> database_,
        std::shared_ptr<std::vector<PatternClique>> multiplicative_patterns_);

    EvaluationResult evaluate(const GlobalState& state);
};

} // namespace maxprob
} // namespace pdbs
} // namespace probabilistic
