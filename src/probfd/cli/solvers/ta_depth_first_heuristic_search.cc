#include "probfd/cli/solvers/ta_depth_first_heuristic_search.h"

#include "language/plugins/internal_function_definition.h"
#include "language/plugins/registry.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search_options.h"
#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/algorithms/open_list.h"
#include "probfd/algorithms/trap_aware_dfhs.h"

#include "probfd/solvers/mdp_heuristic_search.h"

#include <memory>
#include <string>

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms;
using namespace probfd::algorithms::trap_aware_dfhs;

using namespace probfd::cli;
using namespace probfd::cli::solvers;

using namespace language::plugins;

namespace {
using QOpenList = OpenList<quotients::QuotientAction<OperatorID>>;

class TrapAwareDFHSSolver : public MDPHeuristicSearchBase<false, true> {
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
        : MDPHeuristicSearchBase<false, true>(std::forward<Args>(args)...)
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

    std::unique_ptr<StatisticalMDPAlgorithm>
    create_algorithm(const SharedProbabilisticTask& task) override
    {
        return std::make_unique<AlgorithmAdaptor>(
            this->create_search_algorithm<Algorithm>(
                task,
                forward_updates_,
                backward_updates_,
                cutoff_tip_,
                cutoff_inconsistent_,
                labeling_,
                reexpand_traps_));
    }
};

class TrapAwareDFHSSolverFeature
    : public InternalFunctionDefinition<std::shared_ptr<TaskSolverFactory>(
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          Verbosity,
          value_t,
          bool,
          std::shared_ptr<PolicyPickerType<false, true>>,
          std::shared_ptr<QOpenList>,
          bool,
          BacktrackingUpdateType,
          bool,
          bool,
          bool,
          bool)> {
public:
    TrapAwareDFHSSolverFeature()
        : InternalFunctionDefinition("tadfhs", &TrapAwareDFHSSolverFeature::func)
    {
        document_title("Trap-aware depth-first heuristic search family");
        document_synopsis(
            "Supports all MDPs (even non-SSPs) without FRET loop.");

        const auto n =
            add_mdp_hs_base_options_to_feature<false, true>(*this, 7);
        const auto n2 =
            add_base_solver_options_except_algorithm_to_feature(*this, n);

        make_optional_argument_with_default(
            n + n2,
            "open_list",
            add_mdp_type_to_option<false, true>("lifo_open_list()"),
            "Ordering in which successors are considered during policy "
            "exploration.");

        make_optional_argument_with_default(
            n + n2 + 1,
            "fwup",
            "true",
            "Value updates on the way down of exploration.");
        make_optional_argument_with_default(
            n + n2 + 2,
            "bwup",
            "ondemand",
            "Value updates on the way back of exploration");
        make_optional_argument_with_default(
            n + n2 + 3,
            "cutoff_tip",
            "true",
            "Do not follow tip states during policy exploration.");
        make_optional_argument_with_default(
            n + n2 + 4,
            "cutoff_inconsistent",
            "true",
            "Do not expand states whose values have changed during the forward "
            "updates.");
        make_optional_argument_with_default(
            n + n2 + 5,
            "labeling",
            "true",
            "Label states as solved.");
        make_optional_argument_with_default(
            n + n2 + 6,
            "reexpand_traps",
            "true",
            "Immediately re-expand the collapsed trap state.");
    }

protected:
    static std::shared_ptr<TaskSolverFactory> func(
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        Verbosity verbosity,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPickerType<false, true>> policy,
        std::shared_ptr<QOpenList> open_list,
        bool fwup,
        BacktrackingUpdateType bwup,
        bool cutoff_tip,
        bool cutoff_inconsistent,
        bool labeling,
        bool reexpand_traps)
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<TrapAwareDFHSSolver>(
                std::move(open_list),
                fwup,
                bwup,
                cutoff_tip,
                cutoff_inconsistent,
                labeling,
                reexpand_traps,
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

class TrapAwareILAOSolverFeature
    : public InternalFunctionDefinition<std::shared_ptr<TaskSolverFactory>(
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          Verbosity,
          value_t,
          bool,
          std::shared_ptr<PolicyPickerType<false, true>>,
          std::shared_ptr<QOpenList>,
          bool)> {
public:
    TrapAwareILAOSolverFeature()
        : InternalFunctionDefinition("tailao", &TrapAwareILAOSolverFeature::func)
    {
        document_title(
            "iLAO* variant of trap-aware depth-first heuristic search");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);

        const auto n2 =
            add_mdp_hs_base_options_to_feature<false, true>(*this, n);

        make_optional_argument_with_default(
            n + n2,
            "open_list",
            add_mdp_type_to_option<false, true>("lifo_open_list()"),
            "Ordering in which successors are considered during policy "
            "exploration.");

        make_optional_argument_with_default(
            n + n2 + 1,
            "reexpand_traps",
            "true",
            "Immediately re-expand the collapsed trap state.");
    }

    static std::shared_ptr<TaskSolverFactory> func(
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        Verbosity verbosity,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPickerType<false, true>> policy,
        std::shared_ptr<QOpenList> open_list,
        bool reexpand_traps)
    {
        // opts_copy.set<std::string>("name", "ilao");
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<TrapAwareDFHSSolver>(
                std::move(open_list),
                false,
                BacktrackingUpdateType::SINGLE,
                true,
                false,
                false,
                reexpand_traps,
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

class TrapAwareLILAOSolverFeature
    : public InternalFunctionDefinition<std::shared_ptr<TaskSolverFactory>(
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          Verbosity,
          value_t,
          bool,
          std::shared_ptr<PolicyPickerType<false, true>>,
          std::shared_ptr<QOpenList>,
          bool)> {
public:
    TrapAwareLILAOSolverFeature()
        : InternalFunctionDefinition("talilao", &TrapAwareLILAOSolverFeature::func)
    {
        document_title(
            "Labelled iLAO* variant of trap-aware depth-first "
            "heuristic search");

        make_optional_argument_with_default(
            0,
            "open_list",
            add_mdp_type_to_option<false, true>("lifo_open_list()"),
            "Ordering in which successors are considered during policy "
            "exploration.");
        make_optional_argument_with_default(
            1,
            "reexpand_traps",
            "true",
            "Immediately re-expand the collapsed trap state.");

        const auto n =
            add_mdp_hs_base_options_to_feature<false, true>(*this, 2);
        add_base_solver_options_except_algorithm_to_feature(*this, n + 2);
    }

    static std::shared_ptr<TaskSolverFactory> func(
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        Verbosity verbosity,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPickerType<false, true>> policy,
        std::shared_ptr<QOpenList> open_list,
        bool reexpand_traps)
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<TrapAwareDFHSSolver>(
                std::move(open_list),
                false,
                BacktrackingUpdateType::SINGLE,
                true,
                false,
                true,
                reexpand_traps,
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

class TrapAwareHDPSolverFeature
    : public InternalFunctionDefinition<std::shared_ptr<TaskSolverFactory>(
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          Verbosity,
          value_t,
          bool,
          std::shared_ptr<PolicyPickerType<false, true>>,
          std::shared_ptr<QOpenList>,
          bool)> {
public:
    TrapAwareHDPSolverFeature()
        : InternalFunctionDefinition("tahdp", &TrapAwareHDPSolverFeature::func)
    {
        document_title(
            "HDP variant of trap-aware depth-first heuristic search");

        make_optional_argument_with_default(
            0,
            "open_list",
            add_mdp_type_to_option<false, true>("lifo_open_list()"),
            "Ordering in which successors are considered during policy "
            "exploration.");
        make_optional_argument_with_default(
            1,
            "reexpand_traps",
            "true",
            "Immediately re-expand the collapsed trap state.");

        const auto n =
            add_mdp_hs_base_options_to_feature<false, true>(*this, 2);
        add_base_solver_options_except_algorithm_to_feature(*this, n + 2);
    }

    static std::shared_ptr<TaskSolverFactory> func(
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        Verbosity verbosity,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPickerType<false, true>> policy,
        std::shared_ptr<QOpenList> open_list,
        bool reexpand_traps)
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<TrapAwareDFHSSolver>(
                std::move(open_list),
                true,
                BacktrackingUpdateType::ON_DEMAND,
                false,
                true,
                false,
                reexpand_traps,
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

void add_ta_depth_first_heuristic_search_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_declaration<BacktrackingUpdateType>(
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

    n.insert_function_definition<TrapAwareDFHSSolverFeature>();
    n.insert_function_definition<TrapAwareILAOSolverFeature>();
    n.insert_function_definition<TrapAwareLILAOSolverFeature>();
    n.insert_function_definition<TrapAwareHDPSolverFeature>();
}

} // namespace probfd::cli::solvers
