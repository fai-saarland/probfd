#include "probfd/solvers/mdp_solver.h"

#include "probfd/algorithms/exhaustive_dfs.h"

#include "probfd/algorithms/fdr_types.h"
#include "probfd/algorithms/transition_sorter.h"

#include "downward/plugins/plugin.h"

#include "downward/operator_id.h"
#include "downward/task_proxy.h"

#include <memory>
#include <string>

namespace probfd::solvers {
namespace {

using namespace algorithms;
using namespace algorithms::exhaustive_dfs;
using namespace plugins;

class ExhaustiveDFSSolver : public MDPSolver {
    const Interval cost_bound_;

    std::shared_ptr<FDRTransitionSorter> transition_sort_;

    const bool dual_bounds_;
    const bool path_updates_;
    const bool only_propagate_when_changed_;

public:
    explicit ExhaustiveDFSSolver(const Options& opts)
        : MDPSolver(opts)
        , cost_bound_(
              0_vt,
              task_cost_function_->get_non_goal_termination_cost())
        , transition_sort_(
              opts.contains("order")
                  ? opts.get<std::shared_ptr<FDRTransitionSorter>>("order")
                  : nullptr)
        , dual_bounds_(opts.get<bool>("dual_bounds"))
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

        MDPSolver::add_options_to_feature(*this);

        add_option<std::shared_ptr<FDRTransitionSorter>>(
            "order",
            "",
            ArgumentInfo::NO_DEFAULT);

        add_option<bool>("dual_bounds", "", "false");
        add_option<bool>("reverse_path_updates", "", "true");
        add_option<bool>("only_propagate_when_changed", "", "true");
    }
};

} // namespace

static FeaturePlugin<ExhaustiveDFSSolverFeature> _plugin;

} // namespace probfd::solvers
