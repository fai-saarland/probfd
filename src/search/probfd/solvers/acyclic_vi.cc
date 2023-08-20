#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/acyclic_value_iteration.h"

#include "probfd/evaluator.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

class AcyclicVISolver : public MDPSolver {

public:
    using MDPSolver::MDPSolver;

    std::string get_algorithm_name() const override
    {
        return "acyclic_value_iteration";
    }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        using AVIAlgorithm =
            algorithms::acyclic_vi::AcyclicValueIteration<State, OperatorID>;
        return algorithm_factory<AVIAlgorithm>();
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
