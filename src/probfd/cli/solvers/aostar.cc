#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/multi_feature_plugin.h"
#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search.h"
#include "probfd/cli/solvers/mdp_solver.h"

#include "probfd/algorithms/ao_star.h"

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
    create_algorithm(const std::shared_ptr<ProbabilisticTask>& task) override
    {
        return std::make_unique<AlgorithmAdaptor>(
            this->template create_heuristic_search_algorithm<AOStar>(
                task,
                successor_sampler_));
    }
};

template <bool Bisimulation>
class AOStarSolverFeature : public TypedFeature<TaskSolverFactory, MDPSolver> {
    using Sampler = SuccessorSampler<ActionType<Bisimulation, false>>;

public:
    AOStarSolverFeature()
        : TypedFeature<TaskSolverFactory, MDPSolver>(
              add_wrapper_algo_suffix<Bisimulation, false>("aostar"))
    {
        this->document_title("AO* algorithm");

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_options_to_feature<Bisimulation, false>(*this);

        this->template add_option<std::shared_ptr<Sampler>>(
            "successor_sampler",
            "",
            add_mdp_type_to_option<Bisimulation, false>(
                "arbitrary_successor_sampler()"));
    }

protected:
    std::shared_ptr<MDPSolver>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<AOStarSolver<Bisimulation>>(
                options.get<std::shared_ptr<Sampler>>("successor_sampler"),
                get_mdp_hs_args_from_options<Bisimulation, false>(options)),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};

MultiFeaturePlugin<AOStarSolverFeature> _plugin;

} // namespace
