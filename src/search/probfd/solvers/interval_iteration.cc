#include "probfd/solvers/mdp_solver.h"

#include "probfd/engines/interval_iteration.h"

#include "probfd/evaluator.h"
#include "probfd/quotient_system.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

class IntervalIterationSolver : public MDPSolver {
public:
    using MDPSolver::MDPSolver;

    std::string get_engine_name() const override
    {
        return "interval_iteration";
    }

    std::unique_ptr<TaskMDPEngine> create_engine() override
    {
        using IIEngine =
            engines::interval_iteration::IntervalIteration<State, OperatorID>;
        return engine_factory<IIEngine>(false, false);
    }
};

class IntervalIterationSolverFeature
    : public plugins::TypedFeature<SolverInterface, IntervalIterationSolver> {
public:
    IntervalIterationSolverFeature()
        : plugins::TypedFeature<SolverInterface, IntervalIterationSolver>(
              "interval_iteration")
    {
        document_title("Interval Iteration");

        MDPSolver::add_options_to_feature(*this);
    }
};

static plugins::FeaturePlugin<IntervalIterationSolverFeature> _plugin;

} // namespace
} // namespace solvers
} // namespace probfd
