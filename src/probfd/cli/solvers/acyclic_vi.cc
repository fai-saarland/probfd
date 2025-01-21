#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/solvers/mdp_solver.h"

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/acyclic_value_iteration.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <memory>
#include <string>

using namespace utils;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms::acyclic_vi;

using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

namespace {

class AcyclicVISolver : public MDPSolver {
public:
    using MDPSolver::MDPSolver;

    std::string get_algorithm_name() const override
    {
        return "acyclic_value_iteration";
    }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm(
        const std::shared_ptr<ProbabilisticTask>&,
        const std::shared_ptr<FDRCostFunction>&) override
    {
        return std::make_unique<AcyclicValueIteration<State, OperatorID>>();
    }
};

class AcyclicVISolverFeature
    : public TypedFeature<TaskSolverFactory, AcyclicVISolver> {
public:
    AcyclicVISolverFeature()
        : TypedFeature("acyclic_value_iteration")
    {
        document_title("Acyclic Value Iteration");
        add_base_solver_options_to_feature(*this);
    }

protected:
    std::shared_ptr<AcyclicVISolver>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<AcyclicVISolver>(
            get_base_solver_args_from_options(options));
    }
};

FeaturePlugin<AcyclicVISolverFeature> _plugin;

} // namespace
