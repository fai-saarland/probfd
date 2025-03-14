#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/multi_feature_plugin.h"
#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search.h"
#include "probfd/cli/solvers/mdp_solver.h"

#include "probfd/solvers/algorithm_statistics_adaptor.h"

#include "probfd/algorithms/ao_star.h"

#include <memory>
#include <string>

using namespace utils;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms;
using namespace probfd::algorithms::ao_search::ao_star;

using namespace probfd::cli;
using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

namespace {

template <typename State, typename Action>
class AOStarSolver : public MDPHeuristicSearch<State, Action, false> {
    using Sampler = SuccessorSampler<Action>;

    const std::shared_ptr<Sampler> successor_sampler_;

public:
    template <typename... Args>
    explicit AOStarSolver(std::shared_ptr<Sampler> sampler, Args&&... args)
        : MDPHeuristicSearch<State, Action, false>(std::forward<Args>(args)...)
        , successor_sampler_(std::move(sampler))
    {
    }

    std::string get_heuristic_search_name() const override { return "aostar"; }

    std::unique_ptr<StatisticalMDPAlgorithm<State, Action>> create_algorithm(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) override
    {
        return std::make_unique<AlgorithmAdaptor<State, Action>>(
            this->template create_heuristic_search_algorithm<AOStar>(
                task,
                task_cost_function,
                successor_sampler_));
    }
};

template <typename State, typename Action>
class AOStarSolverFactoryFeature
    : public TypedFeature<
          StatisticalMDPAlgorithmFactory<State, Action>,
          AOStarSolver<State, Action>> {
    using Sampler = SuccessorSampler<Action>;

public:
    AOStarSolverFactoryFeature()
        : TypedFeature<
              StatisticalMDPAlgorithmFactory<State, Action>,
              AOStarSolver<State, Action>>("aostar")
    {
        this->document_title("AO* algorithm");

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_options_to_feature<State, Action, false>(*this);

        this->template add_option<std::shared_ptr<Sampler>>(
            "successor_sampler",
            "",
            "arbitrary_successor_sampler()");
    }

protected:
    std::shared_ptr<AOStarSolver<State, Action>>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<AOStarSolver<State, Action>>(
            options.get<std::shared_ptr<Sampler>>("successor_sampler"),
            get_mdp_hs_args_from_options<State, Action, false>(options));
    }
};

class AOStarSolverFeature : public TypedFeature<TaskSolverFactory, MDPSolver> {
    using Sampler = SuccessorSampler<OperatorID>;

public:
    AOStarSolverFeature()
        : TypedFeature<TaskSolverFactory, MDPSolver>("aostar")
    {
        this->document_title("AO* algorithm");

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_options_to_feature<State, OperatorID, false>(*this);

        this->template add_option<std::shared_ptr<Sampler>>(
            "successor_sampler",
            "",
            "arbitrary_successor_sampler()");
    }

protected:
    std::shared_ptr<MDPSolver>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<AOStarSolver<State, OperatorID>>(
                options.get<std::shared_ptr<Sampler>>("successor_sampler"),
                get_mdp_hs_args_from_options<State, OperatorID, false>(
                    options)),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};

FeaturePlugin<AOStarSolverFactoryFeature<State, OperatorID>> _plugin;
FeaturePlugin<
    AOStarSolverFactoryFeature<bisimulation::QuotientState, OperatorID>>
    _plugin2;

FeaturePlugin<AOStarSolverFeature> _solver;

} // namespace
