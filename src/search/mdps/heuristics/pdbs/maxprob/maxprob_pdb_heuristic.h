#pragma once

#include "../../../evaluation_result.h"
#include "../../../state_evaluator.h"
#include "../types.h"
#include "../abstract_state.h"

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
class MaxProbProjection;

class MaxProbPDBHeuristic : public GlobalStateEvaluator {
private:
    enum Multiplicativity {
        NONE = 0, ORTHOGONALITY = 1, WEAK_ORTHOGONALITY = 2
    };

    struct Statistics {
        Multiplicativity multiplicativity = NONE;

        // Database statistics.
        unsigned int abstract_states = 0;
        unsigned int abstract_reachable_states = 0;
        unsigned int abstract_dead_ends = 0;
        unsigned int abstract_one_states = 0;

        unsigned int constructed_patterns = 0;
        unsigned int discarded_patterns = 0;
        unsigned int dead_end_patterns = 0;
        unsigned int clique_patterns = 0;

        // Clique statistics.
        unsigned int num_multiplicative_subcollections = 0;
        double average_multiplicative_subcollection_size = 0;
        std::size_t largest_multiplicative_subcollection_size = 0;

        // Initialization statistics.
        double pattern_construction_time = 0.0;
        double database_construction_time = 0.0;
        double clique_computation_time = 0.0;
        double construction_time = 0.0;

        // Runtime statistics.
        // double evaluate_time = 0.0;
    };

public:
    explicit MaxProbPDBHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    EvaluationResult evaluate(const GlobalState& state) override;

private:
    void dump_construction_statistics(std::ostream& out) const;
    void print_statistics() const override;

    std::shared_ptr<PatternCollection>
    construct_patterns(const options::Options& opts);

    std::vector<Pattern> construct_database(
        const options::Options& opts,
        const PatternCollection& patterns);

    void construct_cliques(
        const options::Options& opts,
        std::vector<Pattern>& clique_patterns);

private:
    bool initial_state_is_dead_end_ = false;

    std::vector<MaxProbProjection> dead_end_database_;
    std::vector<MaxProbProjection> clique_database_;

    std::vector<PatternClique> multiplicative_subcollections;

    Statistics statistics_;
};

} // namespace pdbs
} // namespace probabilistic
