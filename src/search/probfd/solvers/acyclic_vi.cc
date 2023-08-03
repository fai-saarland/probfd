#include "probfd/solvers/mdp_solver.h"

#include "probfd/engines/acyclic_value_iteration.h"

#include "probfd/evaluator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace engine_interfaces;

using AVIEngine = engines::acyclic_vi::AcyclicValueIteration<State, OperatorID>;

class AcyclicVISolver : public MDPSolver {
    std::shared_ptr<TaskEvaluator> prune_;

public:
    explicit AcyclicVISolver(const plugins::Options& opts)
        : MDPSolver(opts)
        , prune_(opts.get<std::shared_ptr<TaskEvaluator>>("eval", nullptr))
    {
    }

    std::string get_engine_name() const override
    {
        return "acyclic_value_iteration";
    }

    std::unique_ptr<TaskMDPEngine> create_engine() override
    {
        return engine_factory<AVIEngine>(prune_.get());
    }
};

class AcyclicVISolverFeature
    : public plugins::TypedFeature<SolverInterface, AcyclicVISolver> {
public:
    AcyclicVISolverFeature()
        : plugins::TypedFeature<SolverInterface, AcyclicVISolver>(
              "acyclic_value_iteration")
    {
        document_title("Acyclic Value Iteration.");

        MDPSolver::add_options_to_feature(*this);

        add_option<std::shared_ptr<TaskEvaluator>>(
            "eval",
            "",
            plugins::ArgumentInfo::NO_DEFAULT);
    }
};

static plugins::FeaturePlugin<AcyclicVISolverFeature> _plugin;

} // namespace
} // namespace solvers
} // namespace probfd
