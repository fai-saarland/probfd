#include "probfd/solvers/mdp_solver.h"

#include "probfd/engines/ta_topological_value_iteration.h"

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

class TATopologicalVISolver : public MDPSolver {
    std::shared_ptr<TaskEvaluator> prune_;

public:
    explicit TATopologicalVISolver(const plugins::Options& opts)
        : MDPSolver(opts)
        , prune_(get_evaluator(opts))
    {
    }

    std::string get_engine_name() const override
    {
        return "ta_topological_value_iteration";
    }

    std::unique_ptr<TaskMDPEngine> create_engine() override
    {
        using TVIEngine = engines::ta_topological_vi::
            TATopologicalValueIteration<State, OperatorID>;
        return engine_factory<TVIEngine>(prune_.get());
    }
};

class TATopologicalVISolverFeature
    : public plugins::TypedFeature<SolverInterface, TATopologicalVISolver> {
public:
    TATopologicalVISolverFeature()
        : plugins::TypedFeature<SolverInterface, TATopologicalVISolver>(
              "ta_topological_value_iteration")
    {
        MDPSolver::add_options_to_feature(*this);

        add_option<std::shared_ptr<TaskEvaluator>>(
            "eval",
            "",
            plugins::ArgumentInfo::NO_DEFAULT);
    }
};

static plugins::FeaturePlugin<TATopologicalVISolverFeature> _plugin;

} // namespace
} // namespace solvers
} // namespace probfd
