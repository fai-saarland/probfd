#include "probfd/cli/solvers/ta_lrtdp.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/raw_registry.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search_options.h"
#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/algorithms/successor_sampler.h"
#include "probfd/algorithms/trap_aware_lrtdp.h"
#include "probfd/solvers/mdp_heuristic_search.h"

#include <memory>
#include <string>

using namespace downward;
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

class TrapAwareLRTDPSolver : public MDPHeuristicSearchBase<false, true> {
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
        : MDPHeuristicSearchBase<false, true>(std::forward<Args>(args)...)
        , successor_sampler_(std::move(successor_sampler))
        , stop_consistent_(terminate_trial)
        , reexpand_traps_(reexpand_traps)
    {
    }

    std::string get_algorithm_name() const override { return "talrtdp"; }

    std::string get_heuristic_search_name() const override { return ""; }

    std::unique_ptr<StatisticalMDPAlgorithm>
    create_algorithm(const SharedProbabilisticTask& task) override
    {
        return std::make_unique<AlgorithmAdaptor>(
            this->template create_search_algorithm<TALRTDP>(
                task,
                stop_consistent_,
                reexpand_traps_,
                successor_sampler_));
    }
};

class TrapAwareLRTDPSolverFeature : public SharedTypedFeature<TaskSolverFactory> {
public:
    TrapAwareLRTDPSolverFeature()
        : SharedTypedFeature("talrtdp")
    {
        document_title("Trap-aware LRTDP");
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

        add_base_solver_options_except_algorithm_to_feature(*this);
        add_mdp_hs_base_options_to_feature<false, true>(*this);
    }

protected:
    std::shared_ptr<TaskSolverFactory>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<TrapAwareLRTDPSolver>(
                options.get_shared<QSuccessorSampler>(
                    "successor_sampler"),
                options.get<TrialTerminationCondition>("terminate_trial"),
                options.get<bool>("reexpand_traps"),
                get_mdp_hs_base_args_from_options<false, true>(options)),
            get_base_solver_args_no_algorithm_from_options(options));
    }
};
}

namespace probfd::cli::solvers {

void add_ta_lrtdp_feature(RawRegistry& raw_registry)
{
    raw_registry.insert_feature_plugin<TrapAwareLRTDPSolverFeature>();

    raw_registry.insert_enum_plugin<TrialTerminationCondition>(
        {{"terminal", "Stop trials at terminal states"},
         {"consistent", "Stop trials at epsilon consistent states"},
         {"inconsistent", "Stop trials at epsilon inconsistent states"},
         {"revisited", "Stop trials upon revisiting a state"}});
}

} // namespace
