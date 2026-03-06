#include "probfd/cli/solvers/exhaustive_dfs.h"

#include "language/ast/internal_function_definition.h"

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

using namespace language::parser;

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

std::shared_ptr<TaskSolverFactory> create_exhaustive_dfs(
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

} // namespace

namespace probfd::cli::solvers {

void add_exhaustive_dfs_feature(NamespaceLevelDeclarationList& nspace)
{
    auto& f = insert_function_definition<create_exhaustive_dfs>(
        nspace,
        "exhaustive_dfs");

    f.document_title("Exhaustive Depth-First Search");

    const auto n = add_base_solver_options_except_algorithm_to_feature(f, 0);

    f.make_optional_argument_with_default(
        n,
        "convergence_epsilon",
        "10e-4",
        "The tolerance for convergence checks.");

    f.make_required_argument(n + 1, "order");

    f.make_optional_argument_with_default(n + 2, "dual_bounds", "false");
    f.make_optional_argument_with_default(
        n + 3,
        "reverse_path_updates",
        "true");
    f.make_optional_argument_with_default(
        n + 4,
        "only_propagate_when_changed",
        "true");
}

} // namespace probfd::cli::solvers
