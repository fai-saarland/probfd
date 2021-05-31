#pragma once

#include "../../../evaluation_result.h"
#include "../../../state_evaluator.h"
#include "../abstract_state.h"
#include "../types.h"
#include "expcost_projection.h"
#include "additive_ecpdbs.h"

#include <memory>
#include <vector>
#include <ostream>

#define ECPDB_MEASURE_EVALUATE

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
class ExpCostAbstractAnalysisResult;

class ExpectedCostPDBHeuristic : public GlobalStateEvaluator {
#if 0
    struct Statistics {
        bool additive = false;

        unsigned int total_states = 0;
        unsigned int num_patterns = 0;

        unsigned int num_additive_subcollections = 0;
        std::size_t average_additive_subcollection_size = 0;
        std::size_t largest_additive_subcollection_size = 0;

        unsigned int num_estimates = 0;
        value_type::value_t average_estimate = 0;
        value_type::value_t lowest_estimate = 0;
        value_type::value_t highest_estimate = -value_type::inf;

        double pattern_construction_time = 0.0;
        double database_construction_time = 0.0;
        double clique_computation_time = 0.0;
        double dominance_pruning_time = 0.0;

        double construction_time = 0.0;
#ifdef ECPDB_MEASURE_EVALUATE
        double evaluate_time = 0.0;
#endif
    };

    Statistics statistics_;
#endif

    AdditiveExpectedCostPDBs additive_ecpds;

public:
    explicit ExpectedCostPDBHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

    void print_statistics() const override;

protected:
    EvaluationResult evaluate(const GlobalState& state) override;
};

} // namespace pdbs
} // namespace probabilistic
