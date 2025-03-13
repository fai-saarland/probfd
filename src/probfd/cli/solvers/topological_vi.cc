#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/solvers/mdp_solver.h"

#include "probfd/solvers/mdp_solver.h"
#include "probfd/solvers/statistical_mdp_algorithm.h"

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

class TopologicalVISolver : public StatisticalMDPAlgorithmFactory {
    const value_t convergence_epsilon_;

public:
    explicit TopologicalVISolver(value_t convergence_epsilon)
        : convergence_epsilon_(convergence_epsilon)
    {
    }

    std::string get_algorithm_name() const override
    {
        return "topological_value_iteration";
    }

    std::unique_ptr<StatisticalMDPAlgorithm> create_algorithm(
        const std::shared_ptr<ProbabilisticTask>&,
        const std::shared_ptr<FDRCostFunction>&) override
    {
        return std::make_unique<AlgorithmAdaptor>(
            std::make_unique<TopologicalValueIteration<State, OperatorID>>(
                convergence_epsilon_,
                false));
    }
};

class TopologicalVISolverFeature
    : public TypedFeature<TaskSolverFactory, MDPSolver> {
public:
    TopologicalVISolverFeature()
        : TypedFeature<TaskSolverFactory, MDPSolver>(
              "topological_value_iteration")
    {
        document_title("Topological Value Iteration");
        add_base_solver_options_except_algorithm_to_feature(*this);

        add_option<value_t>(
            "convergence_epsilon",
            "The tolerance for convergence checks.",
            "10e-4");
    }

protected:
    std::shared_ptr<MDPSolver>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<TopologicalVISolver>(
                options.get<value_t>("convergence_epsilon")),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};

FeaturePlugin<TopologicalVISolverFeature> _plugin;

} // namespace
