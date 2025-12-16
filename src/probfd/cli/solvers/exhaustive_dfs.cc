#include "probfd/cli/solvers/exhaustive_dfs.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/solvers/mdp_solver.h"
#include "probfd/solvers/statistical_mdp_algorithm.h"

#include "probfd/algorithms/exhaustive_dfs.h"
#include "probfd/algorithms/fdr_types.h"
#include "probfd/algorithms/transition_sorter.h"

#include "probfd/probabilistic_task.h"

#include "downward/operator_id.h"
#include "downward/state.h"
#include "probfd/termination_costs.h"

#include <memory>
#include <string>

using namespace downward;
using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::algorithms::exhaustive_dfs;
using namespace probfd::solvers;

using namespace probfd::cli::solvers;

using namespace language::plugins;

namespace {
class ExhaustiveDFSSolver : public StatisticalMDPAlgorithmFactory {
    const value_t convergence_epsilon_;

    const std::shared_ptr<FDRTransitionSorter> transition_sort_;

    const bool dual_bounds_;
    const bool path_updates_;
    const bool only_propagate_when_changed_;

public:
    ExhaustiveDFSSolver(
        value_t convergence_epsilon,
        std::shared_ptr<FDRTransitionSorter> order,
        bool dual_bounds,
        bool reverse_path_updates,
        bool only_propagate_when_changed)
        : convergence_epsilon_(convergence_epsilon)
        , transition_sort_(std::move(order))
        , dual_bounds_(dual_bounds)
        , path_updates_(reverse_path_updates)
        , only_propagate_when_changed_(only_propagate_when_changed)
    {
    }

    std::string get_algorithm_name() const override { return "exhaustive_dfs"; }

    std::unique_ptr<StatisticalMDPAlgorithm>
    create_algorithm(const SharedProbabilisticTask& task) override
    {
        using Algorithm = ExhaustiveDepthFirstSearch<State, OperatorID, false>;
        using Algorithm2 = ExhaustiveDepthFirstSearch<State, OperatorID, true>;

        const auto& term_costs = get_shared_termination_costs(task);

        Interval cost_bound(0_vt, term_costs->get_non_goal_termination_cost());

        if (dual_bounds_) {
            return std::make_unique<AlgorithmAdaptor>(
                std::make_unique<Algorithm2>(
                    convergence_epsilon_,
                    transition_sort_,
                    cost_bound,
                    path_updates_,
                    only_propagate_when_changed_));
        } else {
            return std::make_unique<AlgorithmAdaptor>(
                std::make_unique<Algorithm>(
                    convergence_epsilon_,
                    transition_sort_,
                    cost_bound,
                    path_updates_,
                    only_propagate_when_changed_));
        }
    }
};

class ExhaustiveDFSSolverFeature
    : public InternalFunctionDefinition<std::shared_ptr<TaskSolverFactory>(
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          utils::Verbosity,
          value_t,
          std::shared_ptr<FDRTransitionSorter>,
          bool,
          bool,
          bool)> {
public:
    ExhaustiveDFSSolverFeature()
        : InternalFunctionDefinition("exhaustive_dfs", &ExhaustiveDFSSolverFeature::func)
    {
        document_title("Exhaustive Depth-First Search");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);

        make_optional_argument_with_default(
            n,
            "convergence_epsilon",
            "10e-4",
            "The tolerance for convergence checks.");

        make_required_argument(n + 1, "order");

        make_optional_argument_with_default(n + 2, "dual_bounds", "false");
        make_optional_argument_with_default(
            n + 3,
            "reverse_path_updates",
            "true");
        make_optional_argument_with_default(
            n + 4,
            "only_propagate_when_changed",
            "true");
    }

protected:
    static std::shared_ptr<TaskSolverFactory> func(
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        utils::Verbosity verbosity,
        value_t convergence_epsilon,
        std::shared_ptr<FDRTransitionSorter> order,
        bool dual_bounds,
        bool reverse_path_updates,
        bool only_propagate_when_changed)
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<ExhaustiveDFSSolver>(
                convergence_epsilon,
                std::move(order),
                dual_bounds,
                reverse_path_updates,
                only_propagate_when_changed),
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

void add_exhaustive_dfs_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_function_definition<ExhaustiveDFSSolverFeature>();
}

} // namespace probfd::cli::solvers
