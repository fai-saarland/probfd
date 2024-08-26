#include "probfd_plugins/naming_conventions.h"

#include "probfd_plugins/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/successor_sampler.h"
#include "probfd/algorithms/trap_aware_lrtdp.h"
#include "probfd/solvers/mdp_heuristic_search.h"

#include "downward/plugins/plugin.h"

#include <iostream>
#include <memory>
#include <string>

using namespace plugins;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms;
using namespace probfd::algorithms::trap_aware_lrtdp;

using namespace probfd_plugins;
using namespace probfd_plugins::solvers;

namespace {

using QSuccessorSampler =
    SuccessorSampler<quotients::QuotientAction<OperatorID>>;

class TrapAwareLRTDPSolver : public MDPHeuristicSearch<false, true> {
    const std::shared_ptr<QSuccessorSampler> successor_sampler_;
    const TrialTerminationCondition stop_consistent_;
    const bool reexpand_traps_;

public:
    TrapAwareLRTDPSolver(
        std::shared_ptr<QSuccessorSampler> successor_sampler,
        TrialTerminationCondition terminate_trial,
        bool reexpand_traps,
        bool fret_on_policy,
        bool dual_bounds,
        std::shared_ptr<algorithms::PolicyPicker<
            quotients::QuotientState<State, OperatorID>,
            quotients::QuotientAction<OperatorID>>> policy,
        utils::Verbosity verbosity,
        std::vector<std::shared_ptr<::Evaluator>> path_dependent_evaluators,
        bool cache,
        const std::shared_ptr<TaskEvaluatorFactory>& eval,
        std::optional<value_t> report_epsilon,
        bool report_enabled,
        double max_time,
        std::string policy_filename,
        bool print_fact_names)
        : MDPHeuristicSearch<false, true>(
              fret_on_policy,
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
        , successor_sampler_(std::move(successor_sampler))
        , stop_consistent_(terminate_trial)
        , reexpand_traps_(reexpand_traps)
    {
    }

    std::string get_algorithm_name() const override { return "talrtdp"; }
    std::string get_heuristic_search_name() const override { return ""; }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        return this
            ->template create_quotient_heuristic_search_algorithm<TALRTDP>(
                stop_consistent_,
                reexpand_traps_,
                successor_sampler_);
    }

protected:
    void print_additional_statistics() const override
    {
        MDPHeuristicSearch<false, true>::print_additional_statistics();
        successor_sampler_->print_statistics(std::cout);
    }
};

class TrapAwareLRTDPSolverFeature
    : public TypedFeature<SolverInterface, TrapAwareLRTDPSolver> {
public:
    TrapAwareLRTDPSolverFeature()
        : TypedFeature<SolverInterface, TrapAwareLRTDPSolver>("talrtdp")
    {
        document_title("Trap-aware LRTDP.");
        document_synopsis(
            "Supports all MDP types (even non-SSPs) without FRET loop.");

        add_option<std::shared_ptr<QSuccessorSampler>>(
            "successor_sampler",
            "Successor bias for the trials.",
            add_mdp_type_to_option<false, true>("random_successor_sampler"));
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
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<TrapAwareLRTDPSolver>(
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
