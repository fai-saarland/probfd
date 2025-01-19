#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/multi_feature_plugin.h"
#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search.h"

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

template <bool Bisimulation>
class ExhaustiveAOSolver : public MDPHeuristicSearch<Bisimulation, false> {
    using PolicyPicker =
        typename MDPHeuristicSearch<Bisimulation, false>::PolicyPicker;
    using OpenList = OpenList<ActionType<Bisimulation, false>>;

    const std::shared_ptr<OpenList> open_list_;

public:
    template <typename... Args>
    ExhaustiveAOSolver(std::shared_ptr<OpenList> open_list, Args&&... args)
        : MDPHeuristicSearch<Bisimulation, false>(std::forward<Args>(args)...)
        , open_list_(std::move(open_list))
    {
    }

    std::string get_heuristic_search_name() const override
    {
        return "exhaustive_ao";
    }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) override
    {
        return this->template create_heuristic_search_algorithm<
            algorithms::exhaustive_ao::ExhaustiveAOSearch>(
            task,
            task_cost_function,
            open_list_);
    }
};

template <bool Bisimulation>
class ExhaustiveAOSolverFeature
    : public TypedFeature<TaskSolverFactory, ExhaustiveAOSolver<Bisimulation>> {
    using OpenList = OpenList<ActionType<Bisimulation, false>>;

public:
    ExhaustiveAOSolverFeature()
        : ExhaustiveAOSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, false>("exhaustive_ao"))
    {
        this->document_title("Exhaustive AO* algorithm");

        this->template add_option<std::shared_ptr<OpenList>>(
            "open_list",
            "",
            add_mdp_type_to_option<Bisimulation, false>("lifo_open_list()"));

        add_mdp_hs_options_to_feature<Bisimulation, false>(*this);
    }

protected:
    std::shared_ptr<ExhaustiveAOSolver<Bisimulation>>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<ExhaustiveAOSolver<Bisimulation>>(
            options.get<std::shared_ptr<OpenList>>("open_list"),
            get_mdp_hs_args_from_options<Bisimulation, false>(options));
    }
};

MultiFeaturePlugin<ExhaustiveAOSolverFeature> _plugin;

} // namespace
