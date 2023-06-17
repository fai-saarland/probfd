#include "probfd/solvers/mdp_solver.h"

#include "probfd/engines/interval_iteration.h"

#include "probfd/engine_interfaces/evaluator.h"

#include "probfd/quotient_system.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace engine_interfaces;

class IntervalIterationSolver : public MDPSolver {
    std::shared_ptr<TaskEvaluator> prune_;

public:
    explicit IntervalIterationSolver(const plugins::Options& opts)
        : MDPSolver(opts)
        , prune_(opts.get<std::shared_ptr<TaskEvaluator>>("eval", nullptr))
    {
    }

    std::string get_engine_name() const override
    {
        return "interval_iteration";
    }

    std::unique_ptr<TaskMDPEngineInterface> create_engine() override
    {
        using IIEngine =
            engines::interval_iteration::IntervalIteration<State, OperatorID>;
        return engine_factory<IIEngine>(prune_.get(), false, false);
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

        add_option<std::shared_ptr<TaskEvaluator>>(
            "eval",
            "",
            plugins::ArgumentInfo::NO_DEFAULT);
    }
};

static plugins::FeaturePlugin<IntervalIterationSolverFeature> _plugin;

} // namespace
} // namespace solvers
} // namespace probfd
