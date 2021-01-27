#pragma once

#include "../../../evaluation_result.h"
#include "../../../state_evaluator.h"
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
class AbstractAnalysisResult;

class ExpectedCostPDBHeuristic : public GlobalStateEvaluator {
public:
    explicit ExpectedCostPDBHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    EvaluationResult evaluate(const GlobalState& state) override;

private:
    struct ProjectionInfo {
        ProjectionInfo(
            std::shared_ptr<AbstractStateMapper> state_mapper,
            AbstractAnalysisResult& result);

        std::shared_ptr<AbstractStateMapper> state_mapper;
        std::unique_ptr<QuantitativeResultStore> values;

        value_type::value_t lookup(const AbstractState& s) const;
    };

    std::vector<ProjectionInfo> database_;
};

} // namespace pdbs
} // namespace probabilistic
