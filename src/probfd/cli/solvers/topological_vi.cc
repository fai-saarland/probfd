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
    const value_t convergence_epsilon_;

public:
    TopologicalVISolver(
        utils::Verbosity verbosity,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        std::optional<value_t> report_epsilon,
        bool report_enabled,
        value_t convergence_epsilon)
        : MDPSolver(
              verbosity,
              std::move(task_state_space_factory),
              std::move(heuristic_factory),
              std::move(policy_filename),
              print_fact_names,
              report_epsilon,
              report_enabled)
        , convergence_epsilon_(convergence_epsilon)
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
    : public TypedFeature<TaskSolverFactory, TopologicalVISolver> {
public:
    TopologicalVISolverFeature()
        : TypedFeature<TaskSolverFactory, TopologicalVISolver>(
              "topological_value_iteration")
    {
        document_title("Topological Value Iteration");
        add_base_solver_options_to_feature(*this);

        add_option<value_t>(
            "convergence_epsilon",
            "The tolerance for convergence checks.",
            "10e-4");
    }

protected:
    std::shared_ptr<TopologicalVISolver>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<TopologicalVISolver>(
            get_base_solver_args_from_options(options),
            options.get<value_t>("convergence_epsilon"));
    }
};

FeaturePlugin<TopologicalVISolverFeature> _plugin;

} // namespace
