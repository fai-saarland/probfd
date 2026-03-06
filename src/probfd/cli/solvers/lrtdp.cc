#include "probfd/cli/solvers/lrtdp.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search_options.h"
#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/algorithms/lrtdp.h"

#include "probfd/solvers/mdp_heuristic_search.h"

#include "language/ast/internal_enum_declaration.h"
#include "language/ast/internal_function_definition.h"

#include <memory>
#include <string>

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms;
using namespace probfd::algorithms::lrtdp;

using namespace probfd::cli;
using namespace probfd::cli::solvers;

using namespace language::parser;

namespace {
template <bool Bisimulation, bool Fret>
class LRTDPSolver : public MDPHeuristicSearch<Bisimulation, Fret> {
    using Sampler = SuccessorSampler<ActionType<Bisimulation, Fret>>;

    const std::shared_ptr<Sampler> successor_sampler_;
    const TrialTerminationCondition trial_termination_;

public:
    template <typename... Args>
    LRTDPSolver(
        std::shared_ptr<Sampler> successor_sampler,
        TrialTerminationCondition trial_termination,
        Args&&... args)
        : MDPHeuristicSearch<Bisimulation, Fret>(std::forward<Args>(args)...)
        , successor_sampler_(std::move(successor_sampler))
        , trial_termination_(trial_termination)
    {
    }

    std::string get_heuristic_search_name() const override { return "lrtdp"; }

    std::unique_ptr<StatisticalMDPAlgorithm>
    create_algorithm(const SharedProbabilisticTask& task) override
    {
        return std::make_unique<AlgorithmAdaptor>(
            this->template create_heuristic_search_algorithm<LRTDP>(
                task,
                trial_termination_,
                successor_sampler_));
    }
};

template <bool Bisimulation>
std::shared_ptr<TaskSolverFactory> create_lrtdp(
    std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
    std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
    std::string policy_filename,
    bool print_fact_names,
    value_t report_epsilon,
    bool report_enabled,
    Verbosity verbosity,
    value_t convergence_epsilon,
    bool dual_bounds,
    std::shared_ptr<PolicyPickerType<Bisimulation, false>> policy,
    std::shared_ptr<SuccessorSampler<ActionType<Bisimulation, false>>>
        successor_sampler,
    TrialTerminationCondition trial_termination)
{
    using enum TrialTerminationCondition;

    return make_shared_from_arg_tuples<MDPSolver>(
        make_shared_from_arg_tuples<LRTDPSolver<Bisimulation, false>>(
            std::move(successor_sampler),
            std::move(trial_termination),
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
std::shared_ptr<TaskSolverFactory> create_lrtdp_fret(
    const language::Context& context,
    std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
    std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
    std::string policy_filename,
    bool print_fact_names,
    value_t report_epsilon,
    bool report_enabled,
    Verbosity verbosity,
    bool fret_on_policy,
    value_t convergence_epsilon,
    bool dual_bounds,
    std::shared_ptr<PolicyPickerType<Bisimulation, true>> policy,
    std::shared_ptr<SuccessorSampler<ActionType<Bisimulation, true>>>
        successor_sampler,
    TrialTerminationCondition trial_termination)
{
    using enum TrialTerminationCondition;

    if (trial_termination != CONSISTENT && trial_termination != REVISITED) {
        context.warn(
            "Warning: LRTDP is run within FRET with an unsafe "
            "trial termination condition! LRTDP's trials may "
            "get stuck in cycles.");
    }

    return make_shared_from_arg_tuples<MDPSolver>(
        make_shared_from_arg_tuples<LRTDPSolver<Bisimulation, true>>(
            std::move(successor_sampler),
            std::move(trial_termination),
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
struct AddLRTDPSolverFeatures {
    static void operator()(NamespaceLevelDeclarationList& nspace)
    {
        auto& f = insert_function_definition<create_lrtdp<Bisimulation>>(
            nspace,
            add_wrapper_algo_suffix<Bisimulation, false>("lrtdp"));

        f.document_title("Labelled Real-Time Dynamic Programming");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(f, 0);
        const auto n2 =
            add_mdp_hs_options_to_feature<Bisimulation, false>(f, n);

        f.make_optional_argument_with_default(
            n + n2,
            "successor_sampler",
            add_mdp_type_to_option<Bisimulation, false>(
                "random_successor_sampler()"));

        f.make_optional_argument_with_default(
            n + n2 + 1,
            "trial_termination",
            "terminal");
    }
};

template <bool Bisimulation>
struct AddLRTDPFretSolverFeatures {
    static void operator()(NamespaceLevelDeclarationList& nspace)
    {
        auto& f = insert_function_definition<create_lrtdp_fret<Bisimulation>>(
            nspace,
            add_wrapper_algo_suffix<Bisimulation, true>("lrtdp"));

        f.document_title("Labelled Real-Time Dynamic Programming");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(f, 0);
        const auto n2 = add_mdp_hs_options_to_feature<Bisimulation, true>(f, n);

        f.make_optional_argument_with_default(
            n + n2,
            "successor_sampler",
            add_mdp_type_to_option<Bisimulation, true>(
                "random_successor_sampler()"));

        f.make_optional_argument_with_default(
            n + n2 + 1,
            "trial_termination",
            "terminal");
    }
};

} // namespace

namespace probfd::cli::solvers {

void add_lrtdp_features(NamespaceLevelDeclarationList& nspace)
{
    insert_enum_declaration<TrialTerminationCondition>(
        nspace,
        "TrialTerminationCondition",
        {{"terminal", "Stop trials at terminal states"},
         {"consistent", "Stop trials at epsilon consistent states"},
         {"inconsistent", "Stop trials at epsilon inconsistent states"},
         {"revisited", "Stop trials upon revisiting a state"}});

    insert_function_definitions<AddLRTDPSolverFeatures>(nspace);
    insert_function_definitions<AddLRTDPFretSolverFeatures>(nspace);
}

} // namespace probfd::cli::solvers
