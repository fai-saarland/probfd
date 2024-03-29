#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/exhaustive_dfs.h"

#include "probfd/algorithms/transition_sorter.h"

#include "probfd/transition_sorters/task_transition_sorter_factory.h"

#include "probfd/evaluator.h"
#include "probfd/progress_report.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace algorithms;
using namespace algorithms::exhaustive_dfs;
using namespace plugins;

class ExhaustiveDFSSolver : public MDPSolver {
    const Interval cost_bound_;

    std::shared_ptr<FDRTransitionSorter> transition_sort_;

    const bool dual_bounds_;
    const bool interval_comparison_;
    const bool reevaluate_;
    const bool notify_s0_;
    const bool path_updates_;
    const bool only_propagate_when_changed_;

public:
    explicit ExhaustiveDFSSolver(const Options& opts)
        : MDPSolver(opts)
        , cost_bound_(0_vt, task_cost_function->get_non_goal_termination_cost())
        , transition_sort_(
              opts.contains("order")
                  ? opts.get<std::shared_ptr<FDRTransitionSorterFactory>>(
                            "order")
                        ->create_transition_sorter(this->task_mdp.get())
                  : nullptr)
        , dual_bounds_(opts.get<bool>("dual_bounds"))
        , interval_comparison_(opts.get<bool>("interval_comparison"))
        , reevaluate_(opts.get<bool>("reevaluate"))
        , notify_s0_(opts.get<bool>("initial_state_notification"))
        , path_updates_(opts.get<bool>("reverse_path_updates"))
        , only_propagate_when_changed_(
              opts.get<bool>("only_propagate_when_changed"))
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
                reevaluate_,
                notify_s0_,
                path_updates_,
                only_propagate_when_changed_,
                &progress_);
        } else {
            return std::make_unique<Algorithm>(
                transition_sort_,
                cost_bound_,
                reevaluate_,
                notify_s0_,
                path_updates_,
                only_propagate_when_changed_,
                &progress_);
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

        MDPSolver::add_options_to_feature(*this);

        add_option<std::shared_ptr<FDRTransitionSorterFactory>>(
            "order",
            "",
            ArgumentInfo::NO_DEFAULT);

        add_option<bool>("interval_comparison", "", "false");
        add_option<bool>("dual_bounds", "", "false");
        add_option<bool>("reevaluate", "", "true");
        add_option<bool>("initial_state_notification", "", "false");
        add_option<bool>("reverse_path_updates", "", "true");
        add_option<bool>("only_propagate_when_changed", "", "true");
    }
};

static FeaturePlugin<ExhaustiveDFSSolverFeature> _plugin;

} // namespace
} // namespace solvers
} // namespace probfd
