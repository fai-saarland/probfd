#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search.h"
#include "probfd/cli/solvers/mdp_solver.h"

#include "probfd/algorithms/open_list.h"
#include "probfd/algorithms/trap_aware_dfhs.h"

#include "probfd/solvers/algorithm_statistics_adaptor.h"
#include "probfd/solvers/mdp_heuristic_search.h"

#include <memory>
#include <string>

using namespace utils;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms;
using namespace probfd::algorithms::trap_aware_dfhs;

using namespace probfd::cli;
using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

namespace {

using QOpenList = OpenList<quotients::QuotientAction<OperatorID>>;

class TrapAwareDFHSSolver
    : public MDPHeuristicSearchBase<State, OperatorID, true> {
    template <typename State, typename Action, bool Interval>
    using Algorithm = TADepthFirstHeuristicSearch<State, Action, Interval>;

    const std::shared_ptr<QOpenList> open_list_;
    const bool forward_updates_;
    const BacktrackingUpdateType backward_updates_;
    const bool cutoff_tip_;
    const bool cutoff_inconsistent_;
    const bool labeling_;
    const bool reexpand_traps_;

public:
    template <typename... Args>
    explicit TrapAwareDFHSSolver(
        std::shared_ptr<QOpenList> open_list,
        bool fwup,
        BacktrackingUpdateType bwup,
        bool cutoff_tip,
        bool cutoff_inconsistent,
        bool labeling,
        bool reexpand_traps,
        Args&&... args)
        : MDPHeuristicSearchBase<State, OperatorID, true>(
              std::forward<Args>(args)...)
        , open_list_(std::move(open_list))
        , forward_updates_(fwup)
        , backward_updates_(bwup)
        , cutoff_tip_(cutoff_tip)
        , cutoff_inconsistent_(cutoff_inconsistent)
        , labeling_(labeling)
        , reexpand_traps_(reexpand_traps)
    {
    }

    std::string get_algorithm_name() const override { return "tadfhs"; }

    std::string get_heuristic_search_name() const override { return ""; }

    std::unique_ptr<StatisticalMDPAlgorithm<State, OperatorID>>
    create_algorithm(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) override
    {
        return std::make_unique<AlgorithmAdaptor<State, OperatorID>>(
            this->template create_search_algorithm<Algorithm>(
                task,
                task_cost_function,
                forward_updates_,
                backward_updates_,
                cutoff_tip_,
                cutoff_inconsistent_,
                labeling_,
                reexpand_traps_));
    }
};

class TrapAwareDFHSSolverFeature
    : public TypedFeature<TaskSolverFactory, MDPSolver> {
public:
    TrapAwareDFHSSolverFeature()
        : TypedFeature<TaskSolverFactory, MDPSolver>("tadfhs")
    {
        document_title("Trap-aware depth-first heuristic search family");
        document_synopsis(
            "Supports all MDPs (even non-SSPs) without FRET loop.");

        add_option<std::shared_ptr<QOpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            "lifo_open_list()");

        add_option<bool>(
            "fwup",
            "Value updates on the way down of exploration.",
            "true");
        add_option<BacktrackingUpdateType>(
            "bwup",
            "Value updates on the way back of exploration",
            "ondemand");
        add_option<bool>(
            "cutoff_tip",
            "Do not follow tip states during policy exploration.",
            "true");
        add_option<bool>(
            "cutoff_inconsistent",
            "Do not expand states whose values have changed during the forward "
            "updates.",
            "true");
        add_option<bool>("labeling", "Label states as solved.", "true");
        add_option<bool>(
            "reexpand_traps",
            "Immediately re-expand the collapsed trap state.",
            "true");

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_base_options_to_feature<State, OperatorID, true>(*this);
    }

protected:
    std::shared_ptr<MDPSolver>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<TrapAwareDFHSSolver>(
                options.get<std::shared_ptr<QOpenList>>("open_list"),
                options.get<bool>("fwup"),
                options.get<BacktrackingUpdateType>("bwup"),
                options.get<bool>("cutoff_tip"),
                options.get<bool>("cutoff_inconsistent"),
                options.get<bool>("labeling"),
                options.get<bool>("reexpand_traps"),
                get_mdp_hs_base_args_from_options<State, OperatorID, true>(
                    options)),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};

class TrapAwareILAOSolverFeature
    : public TypedFeature<TaskSolverFactory, MDPSolver> {
public:
    TrapAwareILAOSolverFeature()
        : TypedFeature<TaskSolverFactory, MDPSolver>("tailao")
    {
        document_title(
            "iLAO* variant of trap-aware depth-first heuristic search");

        add_option<std::shared_ptr<QOpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            "lifo_open_list()");
        add_option<bool>(
            "reexpand_traps",
            "Immediately re-expand the collapsed trap state.",
            "true");

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_base_options_to_feature<State, OperatorID, true>(*this);
    }

    std::shared_ptr<MDPSolver>
    create_component(const Options& options, const Context&) const override
    {
        // opts_copy.set<std::string>("name", "ilao");
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<TrapAwareDFHSSolver>(
                options.get<std::shared_ptr<QOpenList>>("open_list"),
                false,
                BacktrackingUpdateType::SINGLE,
                true,
                false,
                false,
                options.get<bool>("reexpand_traps"),
                get_mdp_hs_base_args_from_options<State, OperatorID, true>(
                    options)),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};

class TrapAwareLILAOSolverFeature
    : public TypedFeature<TaskSolverFactory, MDPSolver> {
public:
    TrapAwareLILAOSolverFeature()
        : TypedFeature<TaskSolverFactory, MDPSolver>("talilao")
    {
        document_title(
            "Labelled iLAO* variant of trap-aware depth-first "
            "heuristic search");

        add_option<std::shared_ptr<QOpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            "lifo_open_list()");
        add_option<bool>(
            "reexpand_traps",
            "Immediately re-expand the collapsed trap state.",
            "true");

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_base_options_to_feature<State, OperatorID, true>(*this);
    }

    std::shared_ptr<MDPSolver>
    create_component(const Options& options, const Context&) const override
    {
        // opts_copy.set<std::string>("name", "lilao");
        // opts_copy.set<bool>("labeling", true);

        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<TrapAwareDFHSSolver>(
                options.get<std::shared_ptr<QOpenList>>("open_list"),
                false,
                BacktrackingUpdateType::SINGLE,
                true,
                false,
                true,
                options.get<bool>("reexpand_traps"),
                get_mdp_hs_base_args_from_options<State, OperatorID, true>(
                    options)),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};

class TrapAwareHDPSolverFeature
    : public TypedFeature<TaskSolverFactory, MDPSolver> {
public:
    TrapAwareHDPSolverFeature()
        : TypedFeature<TaskSolverFactory, MDPSolver>("tahdp")
    {
        document_title(
            "HDP variant of trap-aware depth-first heuristic search");

        add_option<std::shared_ptr<QOpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            "lifo_open_list()");
        add_option<bool>(
            "reexpand_traps",
            "Immediately re-expand the collapsed trap state.",
            "true");

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_base_options_to_feature<State, OperatorID, true>(*this);
    }

    std::shared_ptr<MDPSolver>
    create_component(const Options& options, const Context&) const override
    {
        // opts_copy.set<std::string>("name", "hdp");
        // opts_copy.set<bool>("labeling", true);
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<TrapAwareDFHSSolver>(
                options.get<std::shared_ptr<QOpenList>>("open_list"),
                true,
                BacktrackingUpdateType::ON_DEMAND,
                false,
                true,
                false,
                options.get<bool>("reexpand_traps"),
                get_mdp_hs_base_args_from_options<State, OperatorID, true>(
                    options)),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};

FeaturePlugin<TrapAwareDFHSSolverFeature> _plugin_dfhs;
FeaturePlugin<TrapAwareILAOSolverFeature> _plugin_ilao;
FeaturePlugin<TrapAwareLILAOSolverFeature> _plugin_lilao;
FeaturePlugin<TrapAwareHDPSolverFeature> _plugin_hdp;

TypedEnumPlugin<BacktrackingUpdateType> _enum_plugin(
    {{"disabled",
      "No update is performed when backtracking from a state during the dfs "
      "exploration."},
     {"on_demand",
      "An update is performed when backtracking from a state during the dfs "
      "exploration, but only if a previous forward update was made and did not "
      "result in an epsilon-consistent value. Requires forward updates to be "
      "enabled, or tip exploration cutoff to be disabled."},
     {"single",
      "An update is always performed when backtracking from a state during the "
      "dfs exploration."}});

} // namespace
