#include "probfd/solvers/mdp_solver.h"

#include "probfd/engines/acyclic_value_iteration.h"

#include "probfd/evaluator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

class AcyclicVISolver : public MDPSolver {

public:
    using MDPSolver::MDPSolver;

    std::string get_engine_name() const override
    {
        return "acyclic_value_iteration";
    }

    std::unique_ptr<TaskMDPEngine> create_engine() override
    {
        using AVIEngine =
            engines::acyclic_vi::AcyclicValueIteration<State, OperatorID>;
        return engine_factory<AVIEngine>();
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
    }
};

static plugins::FeaturePlugin<AcyclicVISolverFeature> _plugin;

} // namespace
} // namespace solvers
} // namespace probfd
