#pragma once

#include "../../evaluation_result.h"
#include "../../state_evaluator.h"
#include "types.h"
#include "abstract_state.h"

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

    enum Multiplicativity {
        NONE, ORTHOGONALITY, WEAK_ORTHOGONALITY
    };

    struct Statistics {
        int multiplicativity = NONE;

        unsigned abstract_states = 0;
        unsigned abstract_reachable_states = 0;
        unsigned abstract_dead_ends = 0;
        unsigned abstract_one_states = 0;

        unsigned total_projections = 0;

        double init_time = 0.0;
        double clique_init_time = 0.0;
        double eval_time = 0.0;
    };

public:
    explicit MaxProbPDBHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    EvaluationResult evaluate(const GlobalState& state) override;

private:
    void dump_construction_statistics() const;
    void dump_statistics() const;

private:
    bool initial_state_is_dead_end_ = false;

    std::vector<ProjectionInfo> dead_end_database_;
    std::vector<ProjectionInfo> clique_database_;

    std::vector<PatternClique> cliques_;

    Statistics statistics_;
};

} // namespace pdbs
} // namespace probabilistic
