#include "probfd/solvers/mdp_solver.h"

#include "probfd/engines/ta_topological_value_iteration.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/evaluator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

class TATopologicalVISolver : public MDPSolver {
public:
    using MDPSolver::MDPSolver;

    std::string get_engine_name() const override
    {
        return "ta_topological_value_iteration";
    }

    std::unique_ptr<FDRMDPEngine> create_engine() override
    {
        using TVIEngine = engines::ta_topological_vi::
            TATopologicalValueIteration<State, OperatorID>;
        return engine_factory<TVIEngine>();
    }
};

class TATopologicalVISolverFeature
    : public plugins::TypedFeature<SolverInterface, TATopologicalVISolver> {
public:
    TATopologicalVISolverFeature()
        : plugins::TypedFeature<SolverInterface, TATopologicalVISolver>(
              "ta_topological_value_iteration")
    {
        document_title("Trap-Aware Topological Value Iteration.");
        MDPSolver::add_options_to_feature(*this);
    }
};

static plugins::FeaturePlugin<TATopologicalVISolverFeature> _plugin;

} // namespace
} // namespace solvers
} // namespace probfd
