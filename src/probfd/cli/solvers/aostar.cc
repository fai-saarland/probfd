#include "probfd/cli/solvers/aostar.h"

#include "language/ast/internal_function_definition.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search_options.h"
#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/algorithms/ao_star.h"
#include "probfd/solvers/mdp_heuristic_search.h"

#include <memory>
#include <string>

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms;
using namespace probfd::algorithms::ao_search::ao_star;

using namespace probfd::cli;
using namespace probfd::cli::solvers;

using namespace language::parser;

namespace {
template <bool Bisimulation>
class AOStarSolver : public MDPHeuristicSearch<Bisimulation, false> {
    using Sampler = SuccessorSampler<ActionType<Bisimulation, false>>;

    const std::shared_ptr<Sampler> successor_sampler_;

public:
    template <typename... Args>
    explicit AOStarSolver(std::shared_ptr<Sampler> sampler, Args&&... args)
        : MDPHeuristicSearch<Bisimulation, false>(std::forward<Args>(args)...)
        , successor_sampler_(std::move(sampler))
    {
    }

    std::string get_heuristic_search_name() const override { return "aostar"; }

    std::unique_ptr<StatisticalMDPAlgorithm>
    create_algorithm(const SharedProbabilisticTask& task) override
    {
        return std::make_unique<AlgorithmAdaptor>(
            this->template create_heuristic_search_algorithm<AOStar>(
                task,
                successor_sampler_));
    }
};

template <bool Bisimulation>
std::shared_ptr<TaskSolverFactory> create_aostar(
    std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
    std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
    Verbosity verbosity,
    std::string policy_filename,
    bool print_fact_names,
    value_t report_epsilon,
    bool report_enabled,
    value_t convergence_epsilon,
    bool dual_bounds,
    std::shared_ptr<PolicyPickerType<Bisimulation, false>> policy_picker,
    std::shared_ptr<SuccessorSampler<ActionType<Bisimulation, false>>>
        successor_sampler)
{
    return make_shared<MDPSolver>(
        make_shared_from_arg_tuples<AOStarSolver<Bisimulation>>(
            std::move(successor_sampler),
            convergence_epsilon,
            dual_bounds,
            std::move(policy_picker)),
        std::move(task_state_space_factory),
        std::move(heuristic_factory),
        std::move(policy_filename),
        print_fact_names,
        report_epsilon,
        report_enabled,
        verbosity);
}

template <bool Bisimulation>
class AddAOStarSolverFeatures {
public:
    static void operator()(NamespaceLevelDeclarationList& nspace)
    {
        auto& f = insert_function_definition<create_aostar<Bisimulation>>(
            nspace,
            add_wrapper_algo_suffix<Bisimulation, false>("aostar"));

        f.document_title("AO* algorithm");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(f, 0);
        const auto n2 =
            add_mdp_hs_options_to_feature<Bisimulation, false>(f, n);

        f.make_required_argument(
            n + n2,
            "successor_sampler",
            add_mdp_type_to_option<Bisimulation, false>(
                "arbitrary_successor_sampler()"));
    }
};

} // namespace

namespace probfd::cli::solvers {

void add_aostar_solver_features(NamespaceLevelDeclarationList& nspace)
{
    insert_function_definitions<AddAOStarSolverFeatures>(nspace);
}

} // namespace probfd::cli::solvers
