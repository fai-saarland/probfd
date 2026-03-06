#include "probfd/cli/solvers/exhaustive_ao.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search_options.h"
#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/exhaustive_ao.h"

#include <memory>
#include <string>

using namespace downward;
using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::solvers;

using namespace probfd::cli;
using namespace probfd::cli::solvers;

using namespace language::parser;

namespace {
template <bool Bisimulation>
class ExhaustiveAOSolver : public MDPHeuristicSearch<Bisimulation, false> {
    using OpenListType = OpenList<ActionType<Bisimulation, false>>;

    const std::shared_ptr<OpenListType> open_list_;

public:
    template <typename... Args>
    ExhaustiveAOSolver(std::shared_ptr<OpenListType> open_list, Args&&... args)
        : MDPHeuristicSearch<Bisimulation, false>(std::forward<Args>(args)...)
        , open_list_(std::move(open_list))
    {
    }

    std::string get_heuristic_search_name() const override
    {
        return "exhaustive_ao";
    }

    std::unique_ptr<StatisticalMDPAlgorithm>
    create_algorithm(const SharedProbabilisticTask& task) override
    {
        return std::make_unique<AlgorithmAdaptor>(
            this->template create_heuristic_search_algorithm<
                algorithms::exhaustive_ao::ExhaustiveAOSearch>(
                task,
                open_list_));
    }
};

template <bool Bisimulation>
std::shared_ptr<TaskSolverFactory> create_exhaustive_ao(
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
    std::shared_ptr<OpenList<ActionType<Bisimulation, false>>> open_list)
{
    return make_shared_from_arg_tuples<MDPSolver>(
        make_shared_from_arg_tuples<ExhaustiveAOSolver<Bisimulation>>(
            std::move(open_list),
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
class AddExhaustiveAOSolverFeatures {
public:
    static void operator()(NamespaceLevelDeclarationList& nspace)
    {
        auto& f =
            insert_function_definition<create_exhaustive_ao<Bisimulation>>(
                nspace,
                add_wrapper_algo_suffix<Bisimulation, false>("exhaustive_ao"));

        f.document_title("Exhaustive AO* algorithm");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(f, 0);
        const auto n2 =
            add_mdp_hs_options_to_feature<Bisimulation, false>(f, n);

        f.make_optional_argument_with_default(
            n + n2,
            "open_list",
            add_mdp_type_to_option<Bisimulation, false>("lifo_open_list()"));
    }
};

} // namespace

namespace probfd::cli::solvers {

void add_exhaustive_ao_solver_features(NamespaceLevelDeclarationList& nspace)
{
    insert_function_definitions<AddExhaustiveAOSolverFeatures>(nspace);
}

} // namespace probfd::cli::solvers
