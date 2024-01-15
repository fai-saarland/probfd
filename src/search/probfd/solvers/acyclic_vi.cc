#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/acyclic_value_iteration.h"

#include "downward/plugins/plugin.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <memory>
#include <string>

namespace probfd {
namespace solvers {
namespace {

using namespace algorithms::acyclic_vi;
using namespace plugins;

class AcyclicVISolver : public MDPSolver {

public:
    using MDPSolver::MDPSolver;

    std::string get_algorithm_name() const override
    {
        return "acyclic_value_iteration";
    }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        return std::make_unique<AcyclicValueIteration<State, OperatorID>>();
    }
};

class AcyclicVISolverFeature
    : public TypedFeature<SolverInterface, AcyclicVISolver> {
public:
    AcyclicVISolverFeature()
        : TypedFeature("acyclic_value_iteration")
    {
        document_title("Acyclic Value Iteration.");
        MDPSolver::add_options_to_feature(*this);
    }
};

static FeaturePlugin<AcyclicVISolverFeature> _plugin;

} // namespace
} // namespace solvers
} // namespace probfd
