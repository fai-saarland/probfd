#pragma once

#include "../../evaluation_result.h"
#include "../../state_evaluator.h"
#include "../../../pdbs/types.h"
#include "abstract_state.h"
#include "types.h"

#include <memory>
#include <vector>

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

class MaxProbPDBHeuristic : public GlobalStateEvaluator {
private:
    struct ProjectionInfo;

    struct Statistics {
        unsigned abstract_states = 0;
        unsigned abstract_reachable_states = 0;
        unsigned abstract_dead_ends = 0;
        unsigned abstract_one_states = 0;

        unsigned total_projections = 0;
        unsigned stored_projections = 0;
        unsigned deterministic_projections = 0;

        double init_time = 0.0;
        double eval_time = 0.0;

        void dump_initialization_statistics(std::ostream& out) const;
        void dump(std::ostream& out) const;
    };

public:
    explicit MaxProbPDBHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    EvaluationResult evaluate(const GlobalState& state) override;

private:
    bool initial_state_is_dead_end_ = false;
    std::vector<ProjectionInfo> database_;
    Statistics statistics_;
};

} // namespace pdbs
} // namespace probabilistic
