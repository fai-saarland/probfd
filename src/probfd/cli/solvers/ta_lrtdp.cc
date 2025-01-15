#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/successor_sampler.h"
#include "probfd/algorithms/trap_aware_lrtdp.h"
#include "probfd/solvers/mdp_heuristic_search.h"

#include <iostream>
#include <memory>
#include <string>

using namespace utils;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms;
using namespace probfd::algorithms::trap_aware_lrtdp;

using namespace probfd::cli;
using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

namespace {

using QSuccessorSampler =
    SuccessorSampler<quotients::QuotientAction<OperatorID>>;

class TrapAwareLRTDPSolver : public MDPHeuristicSearch<false, true> {
    const std::shared_ptr<QSuccessorSampler> successor_sampler_;
    const TrialTerminationCondition stop_consistent_;
    const bool reexpand_traps_;

public:
    template <typename... Args>
    TrapAwareLRTDPSolver(
        std::shared_ptr<QSuccessorSampler> successor_sampler,
        TrialTerminationCondition terminate_trial,
        bool reexpand_traps,
        Args&&... args)
        : MDPHeuristicSearch<false, true>(std::forward<Args>(args)...)
        , successor_sampler_(std::move(successor_sampler))
        , stop_consistent_(terminate_trial)
        , reexpand_traps_(reexpand_traps)
    {
    }

    std::string get_algorithm_name() const override { return "talrtdp"; }
    std::string get_heuristic_search_name() const override { return ""; }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) override
    {
        return this
            ->template create_quotient_heuristic_search_algorithm<TALRTDP>(
                task,
                task_cost_function,
                stop_consistent_,
                reexpand_traps_,
                successor_sampler_);
    }
};

class TrapAwareLRTDPSolverFeature
    : public TypedFeature<TaskSolverFactory, TrapAwareLRTDPSolver> {
public:
    TrapAwareLRTDPSolverFeature()
        : TypedFeature<TaskSolverFactory, TrapAwareLRTDPSolver>("talrtdp")
    {
        document_title("Trap-aware LRTDP.");
        document_synopsis(
            "Supports all MDP types (even non-SSPs) without FRET loop.");

        add_option<std::shared_ptr<QSuccessorSampler>>(
            "successor_sampler",
            "Successor bias for the trials.",
            add_mdp_type_to_option<false, true>("random_successor_sampler()"));
        add_option<TrialTerminationCondition>(
            "terminate_trial",
            "The trial termination condition.",
            "consistent");
        add_option<bool>(
            "reexpand_traps",
            "Immediately re-expand the collapsed trap state.",
            "true");

        add_mdp_hs_options_to_feature<false, true>(*this);
    }

protected:
    std::shared_ptr<TrapAwareLRTDPSolver>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<TrapAwareLRTDPSolver>(
            options.get<std::shared_ptr<QSuccessorSampler>>(
                "successor_sampler"),
            options.get<TrialTerminationCondition>("terminate_trial"),
            options.get<bool>("reexpand_traps"),
            get_mdp_hs_args_from_options<false, true>(options));
    }
};

FeaturePlugin<TrapAwareLRTDPSolverFeature> _plugin;

TypedEnumPlugin<TrialTerminationCondition> _enum_plugin(
    {{"terminal", "Stop trials at terminal states"},
     {"consistent", "Stop trials at epsilon consistent states"},
     {"inconsistent", "Stop trials at epsilon inconsistent states"},
     {"revisited", "Stop trials upon revisiting a state"}});

} // namespace
