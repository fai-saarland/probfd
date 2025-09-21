#include "probfd/cli/solvers/lrtdp.h"

#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search_options.h"
#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/algorithms/lrtdp.h"

#include "downward/cli/plugins/raw_registry.h"

#include <memory>
#include <string>

using namespace downward;
using namespace utils;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms;
using namespace probfd::algorithms::lrtdp;

using namespace probfd::cli;
using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

namespace {
template <bool Bisimulation, bool Fret>
class LRTDPSolver : public MDPHeuristicSearch<Bisimulation, Fret> {
    using Sampler = SuccessorSampler<ActionType<Bisimulation, Fret>>;

    const std::shared_ptr<Sampler> successor_sampler_;
    const TrialTerminationCondition trial_termination_;

public:
    template <typename... Args>
    LRTDPSolver(
        std::shared_ptr<Sampler> successor_sampler,
        TrialTerminationCondition trial_termination,
        Args&&... args)
        : MDPHeuristicSearch<Bisimulation, Fret>(std::forward<Args>(args)...)
        , successor_sampler_(std::move(successor_sampler))
        , trial_termination_(trial_termination)
    {
    }

    std::string get_heuristic_search_name() const override { return "lrtdp"; }

    std::unique_ptr<StatisticalMDPAlgorithm>
    create_algorithm(const SharedProbabilisticTask& task) override
    {
        return std::make_unique<AlgorithmAdaptor>(
            this->template create_heuristic_search_algorithm<LRTDP>(
                task,
                trial_termination_,
                successor_sampler_));
    }
};

template <bool Bisimulation, bool Fret>
class LRTDPSolverFeature : public TypedFeature<TaskSolverFactory> {
    using Sampler = SuccessorSampler<ActionType<Bisimulation, Fret>>;

public:
    LRTDPSolverFeature()
        : TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, Fret>("lrtdp"))
    {
        this->document_title("Labelled Real-Time Dynamic Programming");

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_options_to_feature<Bisimulation, Fret>(*this);

        this->template add_option<std::shared_ptr<Sampler>>(
            "successor_sampler",
            "",
            add_mdp_type_to_option<Bisimulation, Fret>(
                "random_successor_sampler()"));

        this->template add_option<TrialTerminationCondition>(
            "trial_termination",
            "",
            "terminal");
    }

protected:
    std::shared_ptr<TaskSolverFactory>
    create_component(const Options& options, const Context& context)
        const override
    {
        using enum TrialTerminationCondition;

        const auto trial_termination =
            options.get<TrialTerminationCondition>("trial_termination");

        if constexpr (Fret) {
            if (trial_termination != CONSISTENT &&
                trial_termination != REVISITED) {
                context.warn(
                    "Warning: LRTDP is run within FRET with an unsafe "
                    "trial termination condition! LRTDP's trials may "
                    "get stuck in cycles.");
                }
        }

        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<LRTDPSolver<Bisimulation, Fret>>(
                options.get<std::shared_ptr<Sampler>>("successor_sampler"),
                options.get<TrialTerminationCondition>("trial_termination"),
                get_mdp_hs_args_from_options<Bisimulation, Fret>(options)),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};
}

namespace probfd::cli::solvers {

void add_lrtdp_features(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugins<LRTDPSolverFeature>();

    raw_registry.insert_enum_plugin<TrialTerminationCondition>(
        {{"terminal", "Stop trials at terminal states"},
         {"consistent", "Stop trials at epsilon consistent states"},
         {"inconsistent", "Stop trials at epsilon inconsistent states"},
         {"revisited", "Stop trials upon revisiting a state"}});
}

} // namespace
