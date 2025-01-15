#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/solvers/mdp_solver.h"
#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/topological_value_iteration.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <memory>
#include <string>

using namespace utils;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms::topological_vi;

using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

namespace {

class TopologicalVISolver : public MDPSolver {
public:
    using MDPSolver::MDPSolver;

    std::string get_algorithm_name() const override
    {
        return "topological_value_iteration";
    }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm(
        const std::shared_ptr<ProbabilisticTask>&,
        const std::shared_ptr<FDRCostFunction>&) override
    {
        return std::make_unique<TopologicalValueIteration<State, OperatorID>>(
            false);
    }
};

class TopologicalVISolverFeature
    : public TypedFeature<TaskSolverFactory, TopologicalVISolver> {
public:
    TopologicalVISolverFeature()
        : TypedFeature<TaskSolverFactory, TopologicalVISolver>(
              "topological_value_iteration")
    {
        document_title("Topological Value Iteration.");
        add_base_solver_options_to_feature(*this);
    }

protected:
    std::shared_ptr<TopologicalVISolver>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<TopologicalVISolver>(
            get_base_solver_args_from_options(options));
    }
};

FeaturePlugin<TopologicalVISolverFeature> _plugin;

} // namespace
