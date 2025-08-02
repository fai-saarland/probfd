#include "probfd/cli/solvers/exhaustive_ao.h"

#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search_options.h"
#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/exhaustive_ao.h"

#include "downward/cli/plugins/raw_registry.h"

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

    std::unique_ptr<StatisticalMDPAlgorithm> create_algorithm(
        const SharedProbabilisticTask& task) override
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
    : public TypedFeature<TaskSolverFactory, MDPSolver> {
    using OpenListType = OpenList<ActionType<Bisimulation, false>>;

public:
    ExhaustiveAOSolverFeature()
        : TypedFeature<TaskSolverFactory, MDPSolver>(
              add_wrapper_algo_suffix<Bisimulation, false>("exhaustive_ao"))
    {
        this->document_title("Exhaustive AO* algorithm");

        this->template add_option<std::shared_ptr<OpenListType>>(
            "open_list",
            "",
            add_mdp_type_to_option<Bisimulation, false>("lifo_open_list()"));

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_options_to_feature<Bisimulation, false>(*this);
    }

protected:
    std::shared_ptr<MDPSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<ExhaustiveAOSolver<Bisimulation>>(
                options.get<std::shared_ptr<OpenListType>>("open_list"),
                get_mdp_hs_args_from_options<Bisimulation, false>(options)),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};
}

namespace probfd::cli::solvers {

void add_exhaustive_ao_solver_features(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugins<ExhaustiveAOSolverFeature>();
}

} // namespace
