#include "probfd/cli/solvers/depth_first_heuristic_search.h"

#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search_options.h"
#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/algorithms/depth_first_heuristic_search.h"

#include "downward/cli/plugins/raw_registry.h"

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

    std::unique_ptr<StatisticalMDPAlgorithm>
    create_algorithm(const SharedProbabilisticTask& task) override
    {
        return std::make_unique<AlgorithmAdaptor>(
            this->template create_heuristic_search_algorithm<Algorithm>(
                task,
                forward_updates_,
                backward_updates_,
                cutoff_tip_states_,
                cutoff_inconsistent_,
                labeling_));
    }
};

template <bool Bisimulation, bool Fret>
class DFHSSolverFeature : public SharedTypedFeature<TaskSolverFactory> {
public:
    DFHSSolverFeature()
        : DFHSSolverFeature::SharedTypedFeature(
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
    std::shared_ptr<TaskSolverFactory>
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
class ILAOSolverFeature : public SharedTypedFeature<TaskSolverFactory> {
public:
    ILAOSolverFeature()
        : ILAOSolverFeature::SharedTypedFeature(
              add_wrapper_algo_suffix<Bisimulation, Fret>("ilao"))
    {
        this->document_title("iLAO* variant of depth-first heuristic search");

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_options_to_feature<Bisimulation, Fret>(*this);
    }

    std::shared_ptr<TaskSolverFactory>
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
class LILAOSolverFeature : public SharedTypedFeature<TaskSolverFactory> {
public:
    LILAOSolverFeature()
        : LILAOSolverFeature::SharedTypedFeature(
              add_wrapper_algo_suffix<Bisimulation, Fret>("lilao"))
    {
        this->document_title("Labelled variant of iLAO*");

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_options_to_feature<Bisimulation, Fret>(*this);
    }

    std::shared_ptr<TaskSolverFactory>
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
class HDPSolverFeature : public SharedTypedFeature<TaskSolverFactory> {
public:
    HDPSolverFeature()
        : HDPSolverFeature::SharedTypedFeature(
              add_wrapper_algo_suffix<Bisimulation, Fret>("hdp"))
    {
        this->document_title("HDP variant of depth-first heuristic search");

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_options_to_feature<Bisimulation, Fret>(*this);
    }

    std::shared_ptr<TaskSolverFactory>
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
} // namespace

namespace probfd::cli::solvers {

void add_depth_first_heuristic_search_features(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugins<DFHSSolverFeature>();
    raw_registry.insert_feature_plugins<ILAOSolverFeature>();
    raw_registry.insert_feature_plugins<LILAOSolverFeature>();
    raw_registry.insert_feature_plugins<HDPSolverFeature>();

    raw_registry.insert_enum_plugin<BacktrackingUpdateType>(
        {{"disabled",
          "No update is performed when backtracking from a state during the "
          "dfs "
          "exploration."},
         {"on_demand",
          "An update is performed when backtracking from a state during the "
          "dfs "
          "exploration, but only if a previous forward update was made and did "
          "not "
          "result in an epsilon-consistent value. Requires forward updates to "
          "be "
          "enabled, or tip exploration cutoff to be disabled."},
         {"single",
          "An update is always performed when backtracking from a state during "
          "the "
          "dfs exploration."}});
}

} // namespace probfd::cli::solvers
