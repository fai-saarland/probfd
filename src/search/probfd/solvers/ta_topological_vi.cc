#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/ta_topological_value_iteration.h"

#include "downward/plugins/plugin.h"

#include <memory>
#include <string>

namespace probfd::solvers {
namespace {

using namespace plugins;

class TATopologicalVISolver : public MDPSolver {
public:
    using MDPSolver::MDPSolver;

    std::string get_algorithm_name() const override
    {
        return "ta_topological_value_iteration";
    }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        using TVIAlgorithm = algorithms::ta_topological_vi::
            TATopologicalValueIteration<State, OperatorID>;
        return std::make_unique<TVIAlgorithm>();
    }
};

class TATopologicalVISolverFeature
    : public TypedFeature<SolverInterface, TATopologicalVISolver> {
public:
    TATopologicalVISolverFeature()
        : TypedFeature<SolverInterface, TATopologicalVISolver>(
              "ta_topological_value_iteration")
    {
        document_title("Trap-Aware Topological Value Iteration.");
        MDPSolver::add_options_to_feature(*this);
    }
};

} // namespace

static FeaturePlugin<TATopologicalVISolverFeature> _plugin;

} // namespace probfd::solvers
