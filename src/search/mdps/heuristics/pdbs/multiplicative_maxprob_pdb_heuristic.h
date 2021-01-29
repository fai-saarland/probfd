#pragma once

#include "../../evaluation_result.h"
#include "../../state_evaluator.h"
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

class MultiplicativeMaxProbPDBHeuristic : public GlobalStateEvaluator {
public:
    explicit MultiplicativeMaxProbPDBHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual EvaluationResult evaluate(const GlobalState& state) override;

private:
    using Pattern = std::vector<int>;
    using PatternID = int;
    using PatternClique = std::vector<PatternID>;

    struct ProjectionInfo {
        ProjectionInfo(
            std::shared_ptr<AbstractStateMapper> state_mapper,
            AbstractAnalysisResult& result);

        std::shared_ptr<AbstractStateMapper> state_mapper;
        std::unique_ptr<QuantitativeResultStore> values;
        std::unique_ptr<QualitativeResultStore> dead_ends;
        std::unique_ptr<QualitativeResultStore> one_states;
    };

    void dumpCliqueWithHValues(
        std::ostream& out,
        const GlobalState& state,
        const PatternClique& clique);

    value_type::value_t
    lookup(const ProjectionInfo& info, const AbstractState& s) const;

    bool initial_state_is_dead_end_;
    std::vector<ProjectionInfo> dead_end_database_;

    std::vector<Pattern> clique_patterns_;
    std::vector<ProjectionInfo> clique_database_;
    std::vector<PatternClique> cliques_;

    value_type::value_t one_state_reward_;
};

} // namespace pdbs
} // namespace probabilistic
