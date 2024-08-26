#include "probfd_plugins/multi_feature_plugin.h"
#include "probfd_plugins/naming_conventions.h"

#include "probfd_plugins/solvers/mdp_heuristic_search.h"

#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/exhaustive_ao.h"

#include "downward/plugins/plugin.h"

#include <memory>
#include <string>

using namespace plugins;

using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::solvers;

using namespace probfd_plugins;
using namespace probfd_plugins::solvers;

namespace {

template <bool Bisimulation>
class ExhaustiveAOSolver : public MDPHeuristicSearch<Bisimulation, false> {
    using PolicyPicker =
        typename MDPHeuristicSearch<Bisimulation, false>::PolicyPicker;
    using OpenList = OpenList<ActionType<Bisimulation, false>>;

    const std::shared_ptr<OpenList> open_list_;

public:
    ExhaustiveAOSolver(
        std::shared_ptr<OpenList> open_list,
        bool dual_bounds,
        std::shared_ptr<PolicyPicker> policy,
        utils::Verbosity verbosity,
        std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators,
        bool cache,
        const std::shared_ptr<TaskEvaluatorFactory>& eval,
        std::optional<value_t> report_epsilon,
        bool report_enabled,
        double max_time,
        std::string policy_filename,
        bool print_fact_names)
        : MDPHeuristicSearch<Bisimulation, false>(
              dual_bounds,
              std::move(policy),
              verbosity,
              std::move(path_dependent_evaluators),
              cache,
              eval,
              report_epsilon,
              report_enabled,
              max_time,
              std::move(policy_filename),
              print_fact_names)
        , open_list_(std::move(open_list))
    {
    }

    std::string get_heuristic_search_name() const override
    {
        return "exhaustive_ao";
    }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        return this->template create_heuristic_search_algorithm<
            algorithms::exhaustive_ao::ExhaustiveAOSearch>(open_list_);
    }
};

template <bool Bisimulation>
class ExhaustiveAOSolverFeature
    : public TypedFeature<SolverInterface, ExhaustiveAOSolver<Bisimulation>> {
    using OpenList = OpenList<ActionType<Bisimulation, false>>;

public:
    ExhaustiveAOSolverFeature()
        : ExhaustiveAOSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, false>("exhaustive_ao"))
    {
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
        return plugins::make_shared_from_arg_tuples<
            ExhaustiveAOSolver<Bisimulation>>(
            options.get<std::shared_ptr<OpenList>>("open_list"),
            get_mdp_hs_args_from_options<Bisimulation, false>(options));
    }
};

BinaryFeaturePlugin<ExhaustiveAOSolverFeature> _plugin;

} // namespace
