#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/interval_iteration.h"

#include "probfd/quotients/quotient_system.h"

#include "probfd/evaluator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace algorithms::interval_iteration;
using namespace plugins;

class IntervalIterationSolver : public MDPSolver {
public:
    using MDPSolver::MDPSolver;

    std::string get_algorithm_name() const override
    {
        return "interval_iteration";
    }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        return std::make_unique<IntervalIteration<State, OperatorID>>(
            false,
            false);
    }
};

class IntervalIterationSolverFeature
    : public TypedFeature<SolverInterface, IntervalIterationSolver> {
public:
    IntervalIterationSolverFeature()
        : TypedFeature<SolverInterface, IntervalIterationSolver>(
              "interval_iteration")
    {
        document_title("Interval Iteration");

        MDPSolver::add_options_to_feature(*this);
    }
};

static FeaturePlugin<IntervalIterationSolverFeature> _plugin;

} // namespace
} // namespace solvers
} // namespace probfd
