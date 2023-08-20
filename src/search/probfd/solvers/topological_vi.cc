#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/topological_value_iteration.h"

#include "probfd/heuristics/constant_evaluator.h"

#include "probfd/evaluator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

class TopologicalVISolver : public MDPSolver {
public:
    using MDPSolver::MDPSolver;

    std::string get_algorithm_name() const override
    {
        return "topological_value_iteration";
    }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        using TVIAlgorithm = algorithms::topological_vi::
            TopologicalValueIteration<State, OperatorID>;
        return algorithm_factory<TVIAlgorithm>(false);
    }
};

class TopologicalVISolverFeature
    : public plugins::TypedFeature<SolverInterface, TopologicalVISolver> {
public:
    TopologicalVISolverFeature()
        : plugins::TypedFeature<SolverInterface, TopologicalVISolver>(
              "topological_value_iteration")
    {
        document_title("Topological Value Iteration.");
        MDPSolver::add_options_to_feature(*this);
    }
};

static plugins::FeaturePlugin<TopologicalVISolverFeature> _plugin;

} // namespace
} // namespace solvers
} // namespace probfd
