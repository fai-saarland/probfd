#include "probfd/cli/solvers/depth_first_heuristic_search.h"

#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search_options.h"
#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/algorithms/depth_first_heuristic_search.h"
#include "probfd/solvers/mdp_heuristic_search.h"

#include "downward/cli/plugins/registry.h"

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

template <bool Bisimulation>
class DFHSSolverFeature
    : public SharedTypedFeature<
          TaskSolverFactory,
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          utils::Verbosity,
          value_t,
          bool,
          std::shared_ptr<PolicyPickerType<Bisimulation, false>>,
          bool,
          BacktrackingUpdateType,
          bool,
          bool,
          bool> {
public:
    DFHSSolverFeature()
        : DFHSSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, false>("dfhs"),
              &DFHSSolverFeature::func)
    {
        this->document_title("Depth-first heuristic search family");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);
        const auto n2 =
            add_mdp_hs_options_to_feature<Bisimulation, false>(*this, n);

        this->make_required_argument(n + n2, "labeling");
        this->make_required_argument(n + n2 + 1, "fwup");
        this->make_required_argument(n + n2 + 2, "bwup");
        this->make_required_argument(n + n2 + 3, "cutoff_inconsistent");
        this->make_required_argument(n + n2 + 4, "vi");
        this->make_required_argument(n + n2 + 5, "cutoff_tip");
    }

protected:
    static std::shared_ptr<TaskSolverFactory> func(
        const utils::Context& context,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        utils::Verbosity verbosity,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPickerType<Bisimulation, false>> policy,
        bool forward_updates,
        BacktrackingUpdateType backward_updates,
        bool cutoff_tip,
        bool cutoff_inconsistent,
        bool labeling)
    {
        using enum BacktrackingUpdateType;

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
            make_shared_from_arg_tuples<DFHSSolver<Bisimulation, false>>(
                "dfhs",
                forward_updates,
                backward_updates,
                cutoff_tip,
                cutoff_inconsistent,
                labeling,
                convergence_epsilon,
                dual_bounds,
                std::move(policy)),
            std::move(task_state_space_factory),
            std::move(heuristic_factory),
            std::move(policy_filename),
            print_fact_names,
            report_epsilon,
            report_enabled,
            verbosity);
    }
};

template <bool Bisimulation>
class ILAOSolverFeature
    : public SharedTypedFeature<
          TaskSolverFactory,
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          utils::Verbosity,
          value_t,
          bool,
          std::shared_ptr<PolicyPickerType<Bisimulation, false>>> {
public:
    ILAOSolverFeature()
        : ILAOSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, false>("ilao"),
              &ILAOSolverFeature::func)
    {
        this->document_title("iLAO* variant of depth-first heuristic search");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);

        add_mdp_hs_options_to_feature<Bisimulation, false>(*this, n);
    }

    static std::shared_ptr<TaskSolverFactory> func(
        const utils::Context&,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        utils::Verbosity verbosity,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPickerType<Bisimulation, false>> policy)
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<DFHSSolver<Bisimulation, false>>(
                "ilao",
                false,
                BacktrackingUpdateType::SINGLE,
                true,
                false,
                false,
                convergence_epsilon,
                dual_bounds,
                std::move(policy)),
            std::move(task_state_space_factory),
            std::move(heuristic_factory),
            std::move(policy_filename),
            print_fact_names,
            report_epsilon,
            report_enabled,
            verbosity);
    }
};

template <bool Bisimulation>
class LILAOSolverFeature
    : public SharedTypedFeature<
          TaskSolverFactory,
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          utils::Verbosity,
          value_t,
          bool,
          std::shared_ptr<PolicyPickerType<Bisimulation, false>>> {
public:
    LILAOSolverFeature()
        : LILAOSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, false>("lilao"),
              &LILAOSolverFeature::func)
    {
        this->document_title("Labelled variant of iLAO*");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);
        add_mdp_hs_options_to_feature<Bisimulation, false>(*this, n);
    }

    static std::shared_ptr<TaskSolverFactory> func(
        const utils::Context&,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        utils::Verbosity verbosity,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPickerType<Bisimulation, false>> policy)
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<DFHSSolver<Bisimulation, false>>(
                "lilao",
                false,
                BacktrackingUpdateType::SINGLE,
                true,
                false,
                true,
                convergence_epsilon,
                dual_bounds,
                std::move(policy)),
            std::move(task_state_space_factory),
            std::move(heuristic_factory),
            std::move(policy_filename),
            print_fact_names,
            report_epsilon,
            report_enabled,
            verbosity);
    }
};

template <bool Bisimulation>
class HDPSolverFeature
    : public SharedTypedFeature<
          TaskSolverFactory,
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          utils::Verbosity,
          value_t,
          bool,
          std::shared_ptr<PolicyPickerType<Bisimulation, false>>> {
public:
    HDPSolverFeature()
        : HDPSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, false>("hdp"),
              &HDPSolverFeature::func)
    {
        this->document_title("HDP variant of depth-first heuristic search");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);
        add_mdp_hs_options_to_feature<Bisimulation, false>(*this, n);
    }

    static std::shared_ptr<TaskSolverFactory> func(
        const utils::Context&,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        utils::Verbosity verbosity,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPickerType<Bisimulation, false>> policy)
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<DFHSSolver<Bisimulation, false>>(
                "hdp",
                true,
                BacktrackingUpdateType::ON_DEMAND,
                false,
                true,
                false,
                convergence_epsilon,
                dual_bounds,
                std::move(policy)),
            std::move(task_state_space_factory),
            std::move(heuristic_factory),
            std::move(policy_filename),
            print_fact_names,
            report_epsilon,
            report_enabled,
            verbosity);
    }
};

template <bool Bisimulation>
class DFHSFretSolverFeature
    : public SharedTypedFeature<
          TaskSolverFactory,
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          utils::Verbosity,
          bool,
          value_t,
          bool,
          std::shared_ptr<PolicyPickerType<Bisimulation, true>>,
          bool,
          BacktrackingUpdateType,
          bool,
          bool,
          bool> {
public:
    DFHSFretSolverFeature()
        : DFHSFretSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, true>("dfhs"),
              &DFHSFretSolverFeature::func)
    {
        this->document_title("Depth-first heuristic search family");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);
        const auto n2 =
            add_mdp_hs_options_to_feature<Bisimulation, true>(*this, n);

        this->make_required_argument(n + n2, "labeling");
        this->make_required_argument(n + n2 + 1, "fwup");
        this->make_required_argument(n + n2 + 2, "bwup");
        this->make_required_argument(n + n2 + 3, "cutoff_inconsistent");
        this->make_required_argument(n + n2 + 4, "vi");
        this->make_required_argument(n + n2 + 5, "cutoff_tip");
    }

protected:
    static std::shared_ptr<TaskSolverFactory> func(
        const utils::Context& context,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        utils::Verbosity verbosity,
        bool fret_on_policy,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPickerType<Bisimulation, true>> policy,
        bool forward_updates,
        BacktrackingUpdateType backward_updates,
        bool cutoff_tip,
        bool cutoff_inconsistent,
        bool labeling)
    {
        using enum BacktrackingUpdateType;

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
            make_shared_from_arg_tuples<DFHSSolver<Bisimulation, true>>(
                "dfhs",
                forward_updates,
                backward_updates,
                cutoff_tip,
                cutoff_inconsistent,
                labeling,
                fret_on_policy,
                convergence_epsilon,
                dual_bounds,
                std::move(policy)),
            std::move(task_state_space_factory),
            std::move(heuristic_factory),
            std::move(policy_filename),
            print_fact_names,
            report_epsilon,
            report_enabled,
            verbosity);
    }
};

template <bool Bisimulation>
class ILAOFretSolverFeature
    : public SharedTypedFeature<
          TaskSolverFactory,
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          utils::Verbosity,
          bool,
          value_t,
          bool,
          std::shared_ptr<PolicyPickerType<Bisimulation, true>>> {
public:
    ILAOFretSolverFeature()
        : ILAOFretSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, true>("ilao"),
              &ILAOFretSolverFeature::func)
    {
        this->document_title("iLAO* variant of depth-first heuristic search");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);
        add_mdp_hs_options_to_feature<Bisimulation, true>(*this, n);
    }

    static std::shared_ptr<TaskSolverFactory> func(
        const utils::Context&,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        utils::Verbosity verbosity,
        bool fret_on_policy,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPickerType<Bisimulation, true>> policy)
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<DFHSSolver<Bisimulation, true>>(
                "ilao",
                false,
                BacktrackingUpdateType::SINGLE,
                true,
                false,
                false,
                fret_on_policy,
                convergence_epsilon,
                dual_bounds,
                std::move(policy)),
            std::move(task_state_space_factory),
            std::move(heuristic_factory),
            std::move(policy_filename),
            print_fact_names,
            report_epsilon,
            report_enabled,
            verbosity);
    }
};

template <bool Bisimulation>
class LILAOFretSolverFeature
    : public SharedTypedFeature<
          TaskSolverFactory,
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          utils::Verbosity,
          bool,
          value_t,
          bool,
          std::shared_ptr<PolicyPickerType<Bisimulation, true>>> {
public:
    LILAOFretSolverFeature()
        : LILAOFretSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, true>("lilao"),
              &LILAOFretSolverFeature::func)
    {
        this->document_title("Labelled variant of iLAO*");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);
        add_mdp_hs_options_to_feature<Bisimulation, true>(*this, n);
    }

    static std::shared_ptr<TaskSolverFactory> func(
        const utils::Context&,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        utils::Verbosity verbosity,
        bool fret_on_policy,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPickerType<Bisimulation, true>> policy)
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<DFHSSolver<Bisimulation, true>>(
                "lilao",
                false,
                BacktrackingUpdateType::SINGLE,
                true,
                false,
                true,
                fret_on_policy,
                convergence_epsilon,
                dual_bounds,
                std::move(policy)),
            std::move(task_state_space_factory),
            std::move(heuristic_factory),
            std::move(policy_filename),
            print_fact_names,
            report_epsilon,
            report_enabled,
            verbosity);
    }
};

template <bool Bisimulation>
class HDPFretSolverFeature
    : public SharedTypedFeature<
          TaskSolverFactory,
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          utils::Verbosity,
          bool,
          value_t,
          bool,
          std::shared_ptr<PolicyPickerType<Bisimulation, true>>> {
public:
    HDPFretSolverFeature()
        : HDPFretSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, true>("hdp"),
              &HDPFretSolverFeature::func)
    {
        this->document_title("HDP variant of depth-first heuristic search");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);
        add_mdp_hs_options_to_feature<Bisimulation, true>(*this, n);
    }

    static std::shared_ptr<TaskSolverFactory> func(
        const utils::Context&,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        utils::Verbosity verbosity,
        bool fret_on_policy,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPickerType<Bisimulation, true>> policy)
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<DFHSSolver<Bisimulation, true>>(
                "hdp",
                true,
                BacktrackingUpdateType::ON_DEMAND,
                false,
                true,
                false,
                fret_on_policy,
                convergence_epsilon,
                dual_bounds,
                std::move(policy)),
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

void add_depth_first_heuristic_search_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_plugin<BacktrackingUpdateType>(
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

    n.insert_feature_plugins<DFHSSolverFeature>();
    n.insert_feature_plugins<ILAOSolverFeature>();
    n.insert_feature_plugins<LILAOSolverFeature>();
    n.insert_feature_plugins<HDPSolverFeature>();

    n.insert_feature_plugins<DFHSFretSolverFeature>();
    n.insert_feature_plugins<ILAOFretSolverFeature>();
    n.insert_feature_plugins<LILAOFretSolverFeature>();
    n.insert_feature_plugins<HDPFretSolverFeature>();
}

} // namespace probfd::cli::solvers
