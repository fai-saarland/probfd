#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/solvers/mdp_solver.h"

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/interval_iteration.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <memory>
#include <string>

using namespace utils;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms::interval_iteration;

using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

namespace {

class IntervalIterationSolver : public MDPSolver {
    const value_t convergence_epsilon_;

public:
    IntervalIterationSolver(
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
        return "interval_iteration";
    }

    std::unique_ptr<StatisticalMDPAlgorithm> create_algorithm(
        const std::shared_ptr<ProbabilisticTask>&,
        const std::shared_ptr<FDRCostFunction>&) override
    {
        return std::make_unique<AlgorithmAdaptor>(
            std::make_unique<IntervalIteration<State, OperatorID>>(
                convergence_epsilon_,
                false,
                false));
    }
};

class IntervalIterationSolverFeature
    : public TypedFeature<TaskSolverFactory, IntervalIterationSolver> {
public:
    IntervalIterationSolverFeature()
        : TypedFeature<TaskSolverFactory, IntervalIterationSolver>(
              "interval_iteration")
    {
        document_title("Interval Iteration");

        add_base_solver_options_to_feature(*this);

        add_option<probfd::value_t>(
            "convergence_epsilon",
            "The tolerance for convergence checks.",
            "10e-4");
    }

protected:
    std::shared_ptr<IntervalIterationSolver>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<IntervalIterationSolver>(
            get_base_solver_args_from_options(options),
            options.get<value_t>("convergence_epsilon"));
    }
};

FeaturePlugin<IntervalIterationSolverFeature> _plugin;

} // namespace
