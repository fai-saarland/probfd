#include "probfd/cli/solvers/depth_first_heuristic_search.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search_options.h"
#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/algorithms/depth_first_heuristic_search.h"
#include "probfd/solvers/mdp_heuristic_search.h"

#include "language/ast/compilation_context.h"
#include "language/ast/internal_enum_declaration.h"

#include <memory>
#include <string>

using namespace downward;
using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::algorithms::heuristic_depth_first_search;
using namespace probfd::solvers;

using namespace probfd::cli;
using namespace probfd::cli::solvers;

using namespace language::parser;

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
        using enum BacktrackingUpdateType;

        if (!forward_updates_) {
            if (cutoff_inconsistent) {
                throw std::domain_error(
                    "cutoff_inconsistent requires forward updates!");
            }
            if (backward_updates_ == ON_DEMAND) {
                if (cutoff_tip) {
                    throw std::domain_error(
                        "ondemand backward updates require forward updates or "
                        "cutoff_tip=false!");
                }
            } else if (backward_updates_ == DISABLED && labeling_) {
                throw std::domain_error(
                    "either value_iteration, forward_updates, or "
                    "backward_updates must be enabled!");
            }
        }
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
std::shared_ptr<TaskSolverFactory> create_dfhs(
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

template <bool Bisimulation>
std::shared_ptr<TaskSolverFactory> create_ilao(
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

template <bool Bisimulation>
std::shared_ptr<TaskSolverFactory> create_lilao(
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

template <bool Bisimulation>
std::shared_ptr<TaskSolverFactory> create_hdp(
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

template <bool Bisimulation>
std::shared_ptr<TaskSolverFactory> create_dfhs_fret(
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

template <bool Bisimulation>
std::shared_ptr<TaskSolverFactory> create_ilao_fret(
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

template <bool Bisimulation>
std::shared_ptr<TaskSolverFactory> create_lilao_fret(
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

template <bool Bisimulation>
std::shared_ptr<TaskSolverFactory> create_hdp_fret(
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

template <bool Bisimulation>
class AddDFHSSolverFeatures {
public:
    static void operator()(NamespaceLevelDeclarationList& nspace)
    {
        auto& f = insert_function_definition<create_dfhs<Bisimulation>>(
            nspace,
            add_wrapper_algo_suffix<Bisimulation, false>("dfhs"));

        f.document_title("Depth-first heuristic search family");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(f, 0);
        const auto n2 =
            add_mdp_hs_options_to_feature<Bisimulation, false>(f, n);

        f.make_required_argument(n + n2, "fwup");
        f.make_required_argument(n + n2 + 1, "bwup");
        f.make_required_argument(n + n2 + 2, "cutoff_tip");
        f.make_required_argument(n + n2 + 3, "cutoff_inconsistent");
        f.make_required_argument(n + n2 + 4, "labeling");
    }
};

template <bool Bisimulation>
class AddILAOSolverFeatures {
public:
    static void operator()(NamespaceLevelDeclarationList& nspace)
    {
        auto& f = insert_function_definition<create_ilao<Bisimulation>>(
            nspace,
            add_wrapper_algo_suffix<Bisimulation, false>("ilao"));

        f.document_title("iLAO* variant of depth-first heuristic search");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(f, 0);

        add_mdp_hs_options_to_feature<Bisimulation, false>(f, n);
    }
};

template <bool Bisimulation>
class AddLILAOSolverFeatures {
public:
    static void operator()(NamespaceLevelDeclarationList& nspace)
    {
        auto& f = insert_function_definition<create_ilao<Bisimulation>>(
            nspace,
            add_wrapper_algo_suffix<Bisimulation, false>("lilao"));

        f.document_title("Labelled variant of iLAO*");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(f, 0);
        add_mdp_hs_options_to_feature<Bisimulation, false>(f, n);
    }
};

template <bool Bisimulation>
class AddHDPSolverFeatures {
public:
    static void operator()(NamespaceLevelDeclarationList& nspace)
    {
        auto& f = insert_function_definition<create_ilao<Bisimulation>>(
            nspace,
            add_wrapper_algo_suffix<Bisimulation, false>("hdp"));

        f.document_title("HDP variant of depth-first heuristic search");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(f, 0);
        add_mdp_hs_options_to_feature<Bisimulation, false>(f, n);
    }
};

template <bool Bisimulation>
class AddDFHSFretSolverFeatures {
public:
    static void operator()(NamespaceLevelDeclarationList& nspace)
    {
        auto& f = insert_function_definition<create_dfhs_fret<Bisimulation>>(
            nspace,
            add_wrapper_algo_suffix<Bisimulation, true>("dfhs"));

        f.document_title("Depth-first heuristic search family");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(f, 0);
        const auto n2 = add_mdp_hs_options_to_feature<Bisimulation, true>(f, n);

        f.make_required_argument(n + n2, "fwup");
        f.make_required_argument(n + n2 + 1, "bwup");
        f.make_required_argument(n + n2 + 2, "cutoff_tip");
        f.make_required_argument(n + n2 + 3, "cutoff_inconsistent");
        f.make_required_argument(n + n2 + 4, "labeling");
    }
};

template <bool Bisimulation>
class AddILAOFretSolverFeatures {
public:
    static void operator()(NamespaceLevelDeclarationList& nspace)
    {
        auto& f = insert_function_definition<create_ilao_fret<Bisimulation>>(
            nspace,
            add_wrapper_algo_suffix<Bisimulation, true>("ilao"));

        f.document_title("iLAO* variant of depth-first heuristic search");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(f, 0);
        add_mdp_hs_options_to_feature<Bisimulation, true>(f, n);
    }
};

template <bool Bisimulation>
class AddLILAOFretSolverFeatures {
public:
    static void operator()(NamespaceLevelDeclarationList& nspace)
    {
        auto& f = insert_function_definition<create_lilao_fret<Bisimulation>>(
            nspace,
            add_wrapper_algo_suffix<Bisimulation, true>("lilao"));

        f.document_title("Labelled variant of iLAO*");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(f, 0);
        add_mdp_hs_options_to_feature<Bisimulation, true>(f, n);
    }
};

template <bool Bisimulation>
class AddHDPFretSolverFeatures {
public:
    static void operator()(NamespaceLevelDeclarationList& nspace)
    {
        auto& f = insert_function_definition<create_hdp_fret<Bisimulation>>(
            nspace,
            add_wrapper_algo_suffix<Bisimulation, true>("hdp"));

        f.document_title("HDP variant of depth-first heuristic search");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(f, 0);
        add_mdp_hs_options_to_feature<Bisimulation, true>(f, n);
    }
};

} // namespace

namespace probfd::cli::solvers {

void add_depth_first_heuristic_search_features(
    NamespaceLevelDeclarationList& nspace)
{
    insert_enum_declaration<BacktrackingUpdateType>(
        nspace,
        "BacktrackingUpdateType",
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

    insert_function_definitions<AddDFHSSolverFeatures>(nspace);
    insert_function_definitions<AddILAOSolverFeatures>(nspace);
    insert_function_definitions<AddLILAOSolverFeatures>(nspace);
    insert_function_definitions<AddHDPSolverFeatures>(nspace);

    insert_function_definitions<AddDFHSFretSolverFeatures>(nspace);
    insert_function_definitions<AddILAOFretSolverFeatures>(nspace);
    insert_function_definitions<AddLILAOFretSolverFeatures>(nspace);
    insert_function_definitions<AddHDPFretSolverFeatures>(nspace);
}

} // namespace probfd::cli::solvers
