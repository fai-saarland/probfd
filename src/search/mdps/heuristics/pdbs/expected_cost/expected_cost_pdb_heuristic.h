#pragma once

#include "../../../evaluation_result.h"
#include "../../../state_evaluator.h"
#include "../abstract_state.h"

#include <memory>
#include <vector>
#include <ostream>

class GlobalState;

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {
namespace pdbs {

class ProbabilisticProjection;
class QuantitativeResultStore;
class QualitativeResultStore;
class AbstractAnalysisResult;

class ExpectedCostPDBHeuristic : public GlobalStateEvaluator {
public:
    using PatternID = int;
    using Pattern = std::vector<int>;
    using PatternCollection = std::vector<PatternID>;

private:
    struct Statistics {
        unsigned int total_states = 0;
        unsigned int num_patterns = 0;
        unsigned int num_additive_subcollections = 0;

        unsigned int num_estimates = 0;
        unsigned int num_nontrivial_estimates = 0;
        value_type::value_t average_estimate = 0;
        value_type::value_t lowest_estimate = 0;
        value_type::value_t highest_estimate = 0;
        value_type::value_t initial_estimate = 0;

        double init_time = 0.0;

        void dump(std::ostream& out) const;
    };

    struct ProjectionInfo;

    std::vector<ProjectionInfo> database_;
    std::vector<PatternCollection> additive_patterns_;
    Statistics statistics_;

public:
    explicit ExpectedCostPDBHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    EvaluationResult evaluate(const GlobalState& state) override;
};

} // namespace pdbs
} // namespace probabilistic
