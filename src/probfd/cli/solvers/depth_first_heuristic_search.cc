#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/multi_feature_plugin.h"
#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search.h"
#include "probfd/cli/solvers/mdp_solver.h"

#include "probfd/algorithms/depth_first_heuristic_search.h"

#include <memory>
#include <string>

using namespace downward;
using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::algorithms::heuristic_depth_first_search;
using namespace probfd::solvers;

using namespace probfd::cli;
using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

namespace {

template <bool Bisimulation, bool Fret>
class DFHSSolver : public MDPHeuristicSearch<Bisimulation, Fret> {
    template <typename State, typename Action, bool Interval>
    using Algorithm = HeuristicDepthFirstSearch<State, Action, Interval>;

    const std::string name_;

    const bool forward_updates_;
    const BacktrackingUpdateType backward_updates_;
    const bool cutoff_tip_states_;
    const bool cutoff_inconsistent_;
    const bool labeling_;

public:
    template <typename... Args>
    DFHSSolver(
        std::string variant_name,
        bool fwup,
        BacktrackingUpdateType bwup,
        bool cutoff_tip,
        bool cutoff_inconsistent,
        bool labeling,
        Args&&... args)
        : MDPHeuristicSearch<Bisimulation, Fret>(std::forward<Args>(args)...)
        , name_(std::move(variant_name))
        , forward_updates_(fwup)
        , backward_updates_(bwup)
        , cutoff_tip_states_(cutoff_tip)
        , cutoff_inconsistent_(cutoff_inconsistent)
        , labeling_(labeling)
    {
    }

    std::string get_heuristic_search_name() const override { return name_; }

    std::unique_ptr<StatisticalMDPAlgorithm> create_algorithm(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) override
    {
        return std::make_unique<AlgorithmAdaptor>(
            this->template create_heuristic_search_algorithm<Algorithm>(
                task,
                task_cost_function,
                forward_updates_,
                backward_updates_,
                cutoff_tip_states_,
                cutoff_inconsistent_,
                labeling_));
    }
};

template <bool Bisimulation, bool Fret>
class DFHSSolverFeature : public TypedFeature<TaskSolverFactory, MDPSolver> {
public:
    DFHSSolverFeature()
        : DFHSSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, Fret>("dfhs"))
    {
        this->document_title("Depth-first heuristic search family");

        this->template add_option<bool>(
            "labeling",
            "",
            ArgumentInfo::NO_DEFAULT);
        this->template add_option<bool>("fwup", "", ArgumentInfo::NO_DEFAULT);
        this->template add_option<BacktrackingUpdateType>(
            "bwup",
            "",
            ArgumentInfo::NO_DEFAULT);
        this->template add_option<bool>(
            "cutoff_inconsistent",
            "",
            ArgumentInfo::NO_DEFAULT);
        this->template add_option<bool>("vi", "", ArgumentInfo::NO_DEFAULT);
        this->template add_option<bool>(
            "cutoff_tip",
            "",
            ArgumentInfo::NO_DEFAULT);

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_options_to_feature<Bisimulation, Fret>(*this);
    }

protected:
    std::shared_ptr<MDPSolver>
    create_component(const Options& options, const utils::Context& context)
        const override
    {
        using enum BacktrackingUpdateType;

        bool forward_updates = options.get<bool>("fwup");
        auto backward_updates = options.get<BacktrackingUpdateType>("bwup");
        bool cutoff_tip = options.get<bool>("cutoff_tip");
        bool cutoff_inconsistent = options.get<bool>("cutoff_inconsistent");
        bool labeling = options.get<bool>("labeling");

        if (!forward_updates) {
            if (cutoff_inconsistent) {
                context.error("cutoff_inconsistent requires forward updates!");
            }
            if (backward_updates == ON_DEMAND) {
                if (cutoff_tip) {
                    context.error(
                        "ondemand backward updates require forward updates or "
                        "cutoff_tip=false!");
                }
            } else if (backward_updates == DISABLED && labeling) {
                context.error(
                    "either value_iteration, forward_updates, or "
                    "backward_updates must be enabled!");
            }
        }

        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<DFHSSolver<Bisimulation, Fret>>(
                "dfhs",
                forward_updates,
                backward_updates,
                cutoff_tip,
                cutoff_inconsistent,
                labeling,
                get_mdp_hs_args_from_options<Bisimulation, Fret>(options)),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};

template <bool Bisimulation, bool Fret>
class ILAOSolverFeature : public TypedFeature<TaskSolverFactory, MDPSolver> {
public:
    ILAOSolverFeature()
        : ILAOSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, Fret>("ilao"))
    {
        this->document_title("iLAO* variant of depth-first heuristic search");

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_options_to_feature<Bisimulation, Fret>(*this);
    }

    std::shared_ptr<MDPSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<DFHSSolver<Bisimulation, Fret>>(
                "ilao",
                false,
                BacktrackingUpdateType::SINGLE,
                true,
                false,
                false,
                get_mdp_hs_args_from_options<Bisimulation, Fret>(options)),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};

template <bool Bisimulation, bool Fret>
class LILAOSolverFeature : public TypedFeature<TaskSolverFactory, MDPSolver> {
public:
    LILAOSolverFeature()
        : LILAOSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, Fret>("lilao"))
    {
        this->document_title("Labelled variant of iLAO*");

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_options_to_feature<Bisimulation, Fret>(*this);
    }

    std::shared_ptr<MDPSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<DFHSSolver<Bisimulation, Fret>>(
                "lilao",
                false,
                BacktrackingUpdateType::SINGLE,
                true,
                false,
                true,
                get_mdp_hs_args_from_options<Bisimulation, Fret>(options)),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};

template <bool Bisimulation, bool Fret>
class HDPSolverFeature : public TypedFeature<TaskSolverFactory, MDPSolver> {
public:
    HDPSolverFeature()
        : HDPSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, Fret>("hdp"))
    {
        this->document_title("HDP variant of depth-first heuristic search");

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_options_to_feature<Bisimulation, Fret>(*this);
    }

    std::shared_ptr<MDPSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<DFHSSolver<Bisimulation, Fret>>(
                "hdp",
                true,
                BacktrackingUpdateType::ON_DEMAND,
                false,
                true,
                false,
                get_mdp_hs_args_from_options<Bisimulation, Fret>(options)),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};

MultiFeaturePlugin<DFHSSolverFeature> _plugin_dfhs;
MultiFeaturePlugin<ILAOSolverFeature> _plugin_ilao;
MultiFeaturePlugin<LILAOSolverFeature> _plugin_lilao;
MultiFeaturePlugin<HDPSolverFeature> _plugin_hdp;

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
