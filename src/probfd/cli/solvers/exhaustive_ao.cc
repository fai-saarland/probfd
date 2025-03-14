#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/multi_feature_plugin.h"
#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search.h"
#include "probfd/cli/solvers/mdp_solver.h"

#include "probfd/solvers/algorithm_statistics_adaptor.h"
#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/exhaustive_ao.h"

#include <memory>
#include <string>

using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::solvers;

using namespace probfd::cli;
using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

namespace {

template <typename State, typename Action>
class ExhaustiveAOSolver : public MDPHeuristicSearch<State, Action, false> {
    using OpenListType = OpenList<Action>;

    const std::shared_ptr<OpenListType> open_list_;

public:
    template <typename... Args>
    ExhaustiveAOSolver(std::shared_ptr<OpenListType> open_list, Args&&... args)
        : MDPHeuristicSearch<State, Action, false>(std::forward<Args>(args)...)
        , open_list_(std::move(open_list))
    {
    }

    std::string get_heuristic_search_name() const override
    {
        return "exhaustive_ao";
    }

    std::unique_ptr<StatisticalMDPAlgorithm<State, Action>> create_algorithm(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) override
    {
        return std::make_unique<AlgorithmAdaptor<State, Action>>(
            this->template create_heuristic_search_algorithm<
                algorithms::exhaustive_ao::ExhaustiveAOSearch>(
                task,
                task_cost_function,
                open_list_));
    }
};

template <typename State, typename Action>
class ExhaustiveAOFactoryFeature
    : public TypedFeature<
          StatisticalMDPAlgorithmFactory<State, Action>,
          ExhaustiveAOSolver<State, Action>> {
    using OpenListType = OpenList<ActionType<State, Action, false>>;

public:
    ExhaustiveAOFactoryFeature()
        : TypedFeature<
              StatisticalMDPAlgorithmFactory<State, Action>,
              ExhaustiveAOSolver<State, Action>>("exhaustive_ao")
    {
        this->document_title("Exhaustive AO* algorithm");

        this->template add_option<std::shared_ptr<OpenListType>>(
            "open_list",
            "",
            "lifo_open_list()");

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_options_to_feature<State, Action, false>(*this);
    }

protected:
    std::shared_ptr<ExhaustiveAOSolver<State, Action>>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<ExhaustiveAOSolver<State, Action>>(
            options.get<std::shared_ptr<OpenListType>>("open_list"),
            get_mdp_hs_args_from_options<State, Action, false>(options));
    }
};

class ExhaustiveAOSolverFeature
    : public TypedFeature<TaskSolverFactory, MDPSolver> {
    using OpenListType = OpenList<OperatorID>;

public:
    ExhaustiveAOSolverFeature()
        : TypedFeature<TaskSolverFactory, MDPSolver>("exhaustive_ao")
    {
        this->document_title("Exhaustive AO* algorithm");

        this->template add_option<std::shared_ptr<OpenListType>>(
            "open_list",
            "",
            "lifo_open_list()");

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_options_to_feature<State, OperatorID, false>(*this);
    }

protected:
    std::shared_ptr<MDPSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<ExhaustiveAOSolver<State, OperatorID>>(
                options.get<std::shared_ptr<OpenListType>>("open_list"),
                get_mdp_hs_args_from_options<State, OperatorID, false>(
                    options)),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};

FeaturePlugin<ExhaustiveAOFactoryFeature<State, OperatorID>> _pluginf1;
FeaturePlugin<
    ExhaustiveAOFactoryFeature<bisimulation::QuotientState, OperatorID>>
    _pluginf2;

FeaturePlugin<ExhaustiveAOSolverFeature> _plugin;

} // namespace
