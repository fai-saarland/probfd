#pragma once

#include "../../../evaluation_result.h"
#include "../../../state_evaluator.h"
#include "../../../../utils/printable.h"
#include "../abstract_state.h"
#include "../types.h"
#include "expcost_projection.h"
#include "additive_ecpdbs.h"

#include "pattern_selection/pattern_generator.h"

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
    struct Statistics;

    std::unique_ptr<Statistics> statistics_;
    std::shared_ptr<utils::Printable> generator_report;

    AdditiveExpectedCostPDBs additive_ecpds;

public:
    explicit ExpectedCostPDBHeuristic(const options::Options& opts);

    ExpectedCostPDBHeuristic(
        std::shared_ptr<
            pattern_selection::PatternCollectionGenerator> generator,
        double max_time_dominance_pruning);

    static void add_options_to_parser(options::OptionParser& parser);

    void print_statistics() const override;

protected:
    EvaluationResult evaluate(const GlobalState& state) override;

private:
    AdditiveExpectedCostPDBs
    get_additive_ecpdbs_from_options(
        std::shared_ptr<
            pattern_selection::PatternCollectionGenerator> generator,
        double max_time_dominance_pruning);
};

} // namespace pdbs
} // namespace probabilistic
