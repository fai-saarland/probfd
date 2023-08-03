#include "probfd/solvers/mdp_solver.h"

#include "probfd/engines/topological_value_iteration.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/cost_model.h"
#include "probfd/evaluator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

std::shared_ptr<TaskEvaluator> get_evaluator(const plugins::Options& opts)
{
    if (opts.contains("eval")) {
        return opts.get<std::shared_ptr<TaskEvaluator>>("eval");
    }

    return std::make_shared<heuristics::ConstantEvaluator<State>>(
        g_cost_model->optimal_value_bound().upper);
}

class TopologicalVISolver : public MDPSolver {
    std::shared_ptr<TaskEvaluator> prune_;

public:
    explicit TopologicalVISolver(const plugins::Options& opts)
        : MDPSolver(opts)
        , prune_(get_evaluator(opts))
    {
    }

    std::string get_engine_name() const override
    {
        return "topological_value_iteration";
    }

    std::unique_ptr<TaskMDPEngine> create_engine() override
    {
        using TVIEngine = engines::topological_vi::
            TopologicalValueIteration<State, OperatorID>;
        return engine_factory<TVIEngine>(prune_.get(), false);
    }
};

class TopologicalVISolverFeature
    : public plugins::TypedFeature<SolverInterface, TopologicalVISolver> {
public:
    TopologicalVISolverFeature()
        : plugins::TypedFeature<SolverInterface, TopologicalVISolver>(
              "topological_value_iteration")
    {
        MDPSolver::add_options_to_feature(*this);

        add_option<std::shared_ptr<TaskEvaluator>>(
            "eval",
            "",
            plugins::ArgumentInfo::NO_DEFAULT);
    }
};

static plugins::FeaturePlugin<TopologicalVISolverFeature> _plugin;

} // namespace
} // namespace solvers
} // namespace probfd
