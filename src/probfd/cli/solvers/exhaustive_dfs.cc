#include "probfd/cli/solvers/exhaustive_dfs.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

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

using namespace downward::cli::plugins;

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
    : public TypedFeature<TaskSolverFactory, MDPSolver> {
public:
    ExhaustiveDFSSolverFeature()
        : TypedFeature<TaskSolverFactory, MDPSolver>("exhaustive_dfs")
    {
        document_title("Exhaustive Depth-First Search");

        add_option<value_t>(
            "convergence_epsilon",
            "The tolerance for convergence checks.",
            "10e-4");

        add_option<std::shared_ptr<FDRTransitionSorter>>(
            "order",
            "",
            ArgumentInfo::NO_DEFAULT);

        add_option<bool>("dual_bounds", "", "false");
        add_option<bool>("reverse_path_updates", "", "true");
        add_option<bool>("only_propagate_when_changed", "", "true");

        add_base_solver_options_except_algorithm_to_feature(*this);
    }

protected:
    std::shared_ptr<MDPSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<ExhaustiveDFSSolver>(
                options.get<value_t>("convergence_epsilon"),
                options.get<std::shared_ptr<FDRTransitionSorter>>(
                    "order",
                    nullptr),
                options.get<bool>("dual_bounds"),
                options.get<bool>("reverse_path_updates"),
                options.get<bool>("only_propagate_when_changed")),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};
}

namespace probfd::cli::solvers {

void add_exhaustive_dfs_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<ExhaustiveDFSSolverFeature>();
}

} // namespace
