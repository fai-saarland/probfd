#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/topological_value_iteration.h"

#include "downward/plugins/plugin.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <memory>
#include <string>

namespace probfd::solvers {
namespace {

using namespace algorithms::topological_vi;
using namespace plugins;

class TopologicalVISolver : public MDPSolver {
public:
    using MDPSolver::MDPSolver;

    std::string get_algorithm_name() const override
    {
        return "topological_value_iteration";
    }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        return std::make_unique<TopologicalValueIteration<State, OperatorID>>(
            false);
    }
};

class TopologicalVISolverFeature
    : public TypedFeature<SolverInterface, TopologicalVISolver> {
public:
    TopologicalVISolverFeature()
        : TypedFeature<SolverInterface, TopologicalVISolver>(
              "topological_value_iteration")
    {
        document_title("Topological Value Iteration.");
        MDPSolver::add_options_to_feature(*this);
    }
};

} // namespace

static FeaturePlugin<TopologicalVISolverFeature> _plugin;

} // namespace probfd::solvers
