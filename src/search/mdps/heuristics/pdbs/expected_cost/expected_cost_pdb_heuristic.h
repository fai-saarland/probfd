#pragma once

#include "../../../evaluation_result.h"
#include "../../../state_evaluator.h"
#include "../abstract_state.h"
#include "../types.h"

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
class AbstractAnalysisResult;

class ExpectedCostPDBHeuristic : public GlobalStateEvaluator {
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

    struct ProjectionInfo;

    std::vector<ProjectionInfo> database_;
    std::vector<PatternClique> additive_patterns_;
    Statistics statistics_;

public:
    explicit ExpectedCostPDBHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

    void print_statistics() const override;

protected:
    EvaluationResult evaluate(const GlobalState& state) override;

private:
    void dump_init_statistics(std::ostream &out) const;

    std::shared_ptr<PatternCollection>
    construct_patterns(const options::Options& opts);

    void construct_database(
        const options::Options& opts,
        const PatternCollection& patterns);

    void construct_cliques(
        const options::Options& opts,
        PatternCollection& patterns);

    void run_dominance_pruning(
        double time_limit,
        PatternCollection& patterns);
};

} // namespace pdbs
} // namespace probabilistic
