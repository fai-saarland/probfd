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

class RegroupedOperatorCountingHeuristic : public GlobalStateEvaluator {
public:
    explicit RegroupedOperatorCountingHeuristic(const options::Options& opts);
    static void add_options_to_parser(options::OptionParser& parser);

protected:
    virtual EvaluationResult evaluate(const GlobalState& state) override;

    lp::LPSolver lp_solver_;
    std::vector<int> constraint_offsets_;
};

} // namespace occupation_measure_heuristic
} // namespace probabilistic
