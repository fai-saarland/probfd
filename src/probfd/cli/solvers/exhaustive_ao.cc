#include "probfd/cli/solvers/exhaustive_ao.h"

#include "language/plugins/plugin.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search_options.h"
#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/exhaustive_ao.h"

#include "language/plugins/registry.h"

#include <memory>
#include <string>

using namespace downward;
using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::solvers;

using namespace probfd::cli;
using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

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
class ExhaustiveAOSolverFeature
    : public InternalFunctionDefinition<std::shared_ptr<TaskSolverFactory>(
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
          std::shared_ptr<OpenList<ActionType<Bisimulation, false>>>)> {
    using OpenListType = OpenList<ActionType<Bisimulation, false>>;

public:
    ExhaustiveAOSolverFeature()
        : ExhaustiveAOSolverFeature::InternalFunctionDefinition(
              add_wrapper_algo_suffix<Bisimulation, false>("exhaustive_ao"),
              &ExhaustiveAOSolverFeature::func)
    {
        this->document_title("Exhaustive AO* algorithm");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);
        const auto n2 =
            add_mdp_hs_options_to_feature<Bisimulation, false>(*this, n);

        this->make_optional_argument_with_default(
            n + n2,
            "open_list",
            add_mdp_type_to_option<Bisimulation, false>("lifo_open_list()"));
    }

protected:
    static std::shared_ptr<TaskSolverFactory> func(
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
        std::shared_ptr<OpenListType> open_list)
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
};
} // namespace

namespace probfd::cli::solvers {

void add_exhaustive_ao_solver_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_function_definitions<ExhaustiveAOSolverFeature>();
}

} // namespace probfd::cli::solvers
