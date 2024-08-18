#include "probfd_plugins/solvers/mdp_solver.h"

#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/exhaustive_dfs.h"

#include "probfd/algorithms/fdr_types.h"
#include "probfd/algorithms/transition_sorter.h"

#include "downward/plugins/plugin.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <memory>
#include <string>

using namespace plugins;

using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::algorithms::exhaustive_dfs;
using namespace probfd::solvers;

using namespace probfd_plugins::solvers;

namespace {

class ExhaustiveDFSSolver : public MDPSolver {
    const Interval cost_bound_;

    const std::shared_ptr<FDRTransitionSorter> transition_sort_;

    const bool dual_bounds_;
    const bool path_updates_;
    const bool only_propagate_when_changed_;

public:
    ExhaustiveDFSSolver(
        std::shared_ptr<FDRTransitionSorter> order,
        bool dual_bounds,
        bool reverse_path_updates,
        bool only_propagate_when_changed,
        utils::Verbosity verbosity,
        const std::shared_ptr<TaskCostFunctionFactory>& costs,
        std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators,
        bool cache,
        const std::shared_ptr<TaskEvaluatorFactory>& eval,
        std::optional<value_t> report_epsilon,
        bool report_enabled,
        double max_time,
        std::string policy_filename,
        bool print_fact_names)
        : MDPSolver(
              verbosity,
              costs,
              std::move(path_dependent_evaluators),
              cache,
              eval,
              report_epsilon,
              report_enabled,
              max_time,
              std::move(policy_filename),
              print_fact_names)
        , cost_bound_(
              0_vt,
              task_cost_function_->get_non_goal_termination_cost())
        , transition_sort_(std::move(order))
        , dual_bounds_(dual_bounds)
        , path_updates_(reverse_path_updates)
        , only_propagate_when_changed_(only_propagate_when_changed)
    {
    }

    std::string get_algorithm_name() const override { return "exhaustive_dfs"; }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        using Algorithm = ExhaustiveDepthFirstSearch<State, OperatorID, false>;
        using Algorithm2 = ExhaustiveDepthFirstSearch<State, OperatorID, true>;

        if (dual_bounds_) {
            return std::make_unique<Algorithm2>(
                transition_sort_,
                cost_bound_,
                path_updates_,
                only_propagate_when_changed_);
        } else {
            return std::make_unique<Algorithm>(
                transition_sort_,
                cost_bound_,
                path_updates_,
                only_propagate_when_changed_);
        }
    }
};

class ExhaustiveDFSSolverFeature
    : public TypedFeature<SolverInterface, ExhaustiveDFSSolver> {
public:
    ExhaustiveDFSSolverFeature()
        : TypedFeature<SolverInterface, ExhaustiveDFSSolver>("exhaustive_dfs")
    {
        document_title("Exhaustive Depth-First Search");

        add_option<std::shared_ptr<FDRTransitionSorter>>(
            "order",
            "",
            ArgumentInfo::NO_DEFAULT);

        add_option<bool>("dual_bounds", "", "false");
        add_option<bool>("reverse_path_updates", "", "true");
        add_option<bool>("only_propagate_when_changed", "", "true");

        add_base_solver_options_to_feature(*this);
    }

protected:
    std::shared_ptr<ExhaustiveDFSSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<ExhaustiveDFSSolver>(
            options.get<std::shared_ptr<FDRTransitionSorter>>("order", nullptr),
            options.get<bool>("dual_bounds"),
            options.get<bool>("reverse_path_updates"),
            options.get<bool>("only_propagate_when_changed"),
            get_base_solver_args_from_options(options));
    }
};

FeaturePlugin<ExhaustiveDFSSolverFeature> _plugin;

} // namespace
