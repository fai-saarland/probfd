#pragma once

#include "../../../lp/lp_solver.h"
#include "../../state_evaluator.h"

#include <vector>

namespace options {
class Options;
class OptionParser;
} // namespace options

namespace probabilistic {
namespace occupation_measure_heuristic {

class ProjectionOccupationMeasureHeuristic : public GlobalStateEvaluator {
public:
    explicit ProjectionOccupationMeasureHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

    static void generate_hpom_lp(
            lp::LPSolver& lp_solver,
            std::vector<lp::LPVariable>& vars,
            std::vector<lp::LPConstraint>& constraints,
            std::vector<int>& offsets,
            std::vector<int>& goal);

protected:
    virtual EvaluationResult evaluate(const GlobalState& state) override;

    lp::LPSolver lp_solver_;
    std::vector<int> offset_;
    std::vector<int> goal_;
};

} // namespace occupation_measure_heuristic
} // namespace probabilistic
