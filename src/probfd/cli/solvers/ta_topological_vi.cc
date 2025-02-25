#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/solvers/mdp_solver.h"

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/ta_topological_value_iteration.h"

#include <memory>
#include <string>

using namespace probfd;
using namespace probfd::solvers;

using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

namespace {

class TATopologicalVISolver : public MDPSolver {
    const value_t convergence_epsilon_;

public:
    TATopologicalVISolver(
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
        return "ta_topological_value_iteration";
    }

    std::unique_ptr<StatisticalMDPAlgorithm> create_algorithm(
        const std::shared_ptr<ProbabilisticTask>&,
        const std::shared_ptr<FDRCostFunction>&) override
    {
        using TVIAlgorithm = algorithms::ta_topological_vi::
            TATopologicalValueIteration<State, OperatorID>;
        return std::make_unique<AlgorithmAdaptor>(
            std::make_unique<TVIAlgorithm>(convergence_epsilon_));
    }
};

class TATopologicalVISolverFeature
    : public TypedFeature<TaskSolverFactory, TATopologicalVISolver> {
public:
    TATopologicalVISolverFeature()
        : TypedFeature<TaskSolverFactory, TATopologicalVISolver>(
              "ta_topological_value_iteration")
    {
        document_title("Trap-Aware Topological Value Iteration");
        add_base_solver_options_to_feature(*this);

        add_option<value_t>(
            "convergence_epsilon",
            "The tolerance for convergence checks.",
            "10e-4");
    }

protected:
    std::shared_ptr<TATopologicalVISolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<TATopologicalVISolver>(
            get_base_solver_args_from_options(options),
            options.get<value_t>("convergence_epsilon"));
    }
};

FeaturePlugin<TATopologicalVISolverFeature> _plugin;

} // namespace
