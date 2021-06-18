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
    struct Statistics {
        size_t num_patterns = 0;
        size_t total_states = 0;
        size_t num_additive_subcollections = 0;

        double dominance_pruning_time = 0.0;
        double construction_time = 0.0;

        void print(std::ostream& out) const;
    };

    Statistics statistics_;

    AdditiveExpectedCostPDBs additive_ecpds;

public:
    explicit ExpectedCostPDBHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

    void print_statistics() const override;

protected:
    EvaluationResult evaluate(const GlobalState& state) override;

private:
    AdditiveExpectedCostPDBs
    get_additive_ecpdbs_from_options(const options::Options &opts);
};

} // namespace pdbs
} // namespace probabilistic
