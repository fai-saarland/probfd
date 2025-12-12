#include "probfd/cli/solvers/topological_vi.h"

#include "language/plugins/plugin.h"
#include "language/plugins/registry.h"

#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/solvers/mdp_solver.h"
#include "probfd/solvers/statistical_mdp_algorithm.h"

#include "probfd/algorithms/topological_value_iteration.h"

#include "downward/operator_id.h"
#include "downward/state.h"

#include <memory>
#include <string>

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms::topological_vi;

using namespace probfd::cli::solvers;

using namespace language::plugins;

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

    std::unique_ptr<StatisticalMDPAlgorithm>
    create_algorithm(const SharedProbabilisticTask&) override
    {
        return std::make_unique<AlgorithmAdaptor>(
            std::make_unique<TopologicalValueIteration<State, OperatorID>>(
                convergence_epsilon_,
                false));
    }
};

class TopologicalVISolverFeature
    : public InternalFunctionDefinition<std::shared_ptr<TaskSolverFactory>(
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          Verbosity,
          value_t)> {
public:
    TopologicalVISolverFeature()
        : InternalFunctionDefinition(
              "topological_value_iteration",
              &TopologicalVISolverFeature::func)
    {
        document_title("Topological Value Iteration");
        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);

        make_optional_argument_with_default(
            n,
            "convergence_epsilon",
            "10e-4",
            "The tolerance for convergence checks.");
    }

protected:
    static std::shared_ptr<TaskSolverFactory> func(
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        Verbosity verbosity,
        value_t convergence_epsilon)
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<TopologicalVISolver>(
                convergence_epsilon),
            std::move(task_state_space_factory),
            std::move(heuristic_factory),
            std::move(policy_filename),
            print_fact_names,
            report_epsilon,
            report_enabled,
            verbosity);
    }
};
} // namespace

namespace probfd::cli::solvers {

void add_topological_value_iteration_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_function_definition<TopologicalVISolverFeature>();
}

} // namespace probfd::cli::solvers
