#include "probfd/cli/solvers/aostar.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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

using namespace downward::cli::plugins;

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
class AOStarSolverFeature
    : public SharedTypedFeature<
          TaskSolverFactory,
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          Verbosity,
          std::string,
          bool,
          value_t,
          bool,
          value_t,
          bool,
          std::shared_ptr<PolicyPickerType<Bisimulation, false>>,
          std::shared_ptr<SuccessorSampler<ActionType<Bisimulation, false>>>> {
    using Sampler = SuccessorSampler<ActionType<Bisimulation, false>>;

public:
    AOStarSolverFeature()
        : AOStarSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, false>("aostar"),
              &AOStarSolverFeature::func)
    {
        this->document_title("AO* algorithm");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);
        const auto n2 =
            add_mdp_hs_options_to_feature<Bisimulation, false>(*this, n);

        this->make_required_argument(
            n + n2,
            "successor_sampler",
            add_mdp_type_to_option<Bisimulation, false>(
                "arbitrary_successor_sampler()"));
    }

protected:
    static std::shared_ptr<TaskSolverFactory> func(
        const Context&,
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
        std::shared_ptr<Sampler> successor_sampler)
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
};
} // namespace

namespace probfd::cli::solvers {

void add_aostar_solver_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_feature_plugins<AOStarSolverFeature>();
}

} // namespace probfd::cli::solvers
