#include "probfd_plugins/multi_feature_plugin.h"
#include "probfd_plugins/naming_conventions.h"

#include "probfd_plugins/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/lrtdp.h"

#include "downward/plugins/plugin.h"

#include <iostream>
#include <memory>
#include <string>

using namespace plugins;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms;
using namespace probfd::algorithms::lrtdp;

using namespace probfd_plugins;
using namespace probfd_plugins::solvers;

namespace {

template <bool Bisimulation, bool Fret>
class LRTDPSolver : public MDPHeuristicSearch<Bisimulation, Fret> {
    template <typename State, typename Action, bool Interval>
    using LRTDP = LRTDP<State, Action, Interval>;

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

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        return this->template create_heuristic_search_algorithm<LRTDP>(
            trial_termination_,
            successor_sampler_);
    }

protected:
    void print_additional_statistics() const override
    {
        MDPHeuristicSearch<Bisimulation, Fret>::print_additional_statistics();
        successor_sampler_->print_statistics(std::cout);
    }
};

template <bool Bisimulation, bool Fret>
class LRTDPSolverFeature
    : public TypedFeature<SolverInterface, LRTDPSolver<Bisimulation, Fret>> {
    using Sampler = SuccessorSampler<ActionType<Bisimulation, Fret>>;

public:
    LRTDPSolverFeature()
        : TypedFeature<SolverInterface, LRTDPSolver<Bisimulation, Fret>>(
              add_wrapper_algo_suffix<Bisimulation, Fret>("lrtdp"))
    {
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
    std::shared_ptr<LRTDPSolver<Bisimulation, Fret>>
    create_component(const Options& options, const utils::Context& context)
        const override
    {
        using enum TrialTerminationCondition;

        const auto trial_termination =
            options.get<TrialTerminationCondition>("trial_termination");

        if constexpr (Fret) {
            if (trial_termination != CONSISTENT &&
                trial_termination != REVISITED) {
                context.warn("Warning: LRTDP is run within FRET with an unsafe "
                             "trial termination condition! LRTDP's trials may "
                             "get stuck in cycles.");
            }
        }

        return plugins::make_shared_from_arg_tuples<
            LRTDPSolver<Bisimulation, Fret>>(
            options.get<std::shared_ptr<Sampler>>("successor_sampler"),
            options.get<TrialTerminationCondition>("trial_termination"),
            get_mdp_hs_args_from_options<Bisimulation, Fret>(options));
    }
};

MultiFeaturePlugin<LRTDPSolverFeature> _plugins;

TypedEnumPlugin<TrialTerminationCondition> _enum_plugin(
    {{"terminal", "Stop trials at terminal states"},
     {"consistent", "Stop trials at epsilon consistent states"},
     {"inconsistent", "Stop trials at epsilon inconsistent states"},
     {"revisited", "Stop trials upon revisiting a state"}});

} // namespace
