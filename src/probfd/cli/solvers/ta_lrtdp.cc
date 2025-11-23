#include "probfd/cli/solvers/ta_lrtdp.h"

#include "downward/cli/plugins/plugin.h"
#include "downward/cli/plugins/registry.h"

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

class TrapAwareLRTDPSolverFeature
    : public InternalFunctionDefinition<std::shared_ptr<TaskSolverFactory>(
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          Verbosity,
          std::string,
          bool,
          value_t,
          bool,
          value_t,
          bool,
          std::shared_ptr<PolicyPickerType<false, true>>,
          std::shared_ptr<QSuccessorSampler>,
          TrialTerminationCondition,
          bool)> {
public:
    TrapAwareLRTDPSolverFeature()
        : InternalFunctionDefinition("talrtdp", &TrapAwareLRTDPSolverFeature::func)
    {
        document_title("Trap-aware LRTDP");
        document_synopsis(
            "Supports all MDP types (even non-SSPs) without FRET loop.");

        make_optional_argument_with_default(
            0,
            "successor_sampler",
            add_mdp_type_to_option<false, true>("random_successor_sampler()"),
            "Successor bias for the trials.");

        make_optional_argument_with_default(
            1,
            "terminate_trial",
            "consistent",
            "The trial termination condition.");

        make_optional_argument_with_default(
            2,
            "reexpand_traps",
            "true",
            "Immediately re-expand the collapsed trap state.");

        const auto n =
            add_mdp_hs_base_options_to_feature<false, true>(*this, 3);
        add_base_solver_options_except_algorithm_to_feature(*this, n + 3);
    }

protected:
    static std::shared_ptr<TaskSolverFactory> func(
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        Verbosity verbosity,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPickerType<false, true>> policy_picker,
        std::shared_ptr<QSuccessorSampler> successor_sampler,
        TrialTerminationCondition terminate_trial,
        bool reexpand_traps)
    {
        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<TrapAwareLRTDPSolver>(
                std::move(successor_sampler),
                terminate_trial,
                reexpand_traps,
                convergence_epsilon,
                dual_bounds,
                std::move(policy_picker)),
            std::move(task_state_space_factory),
            std::move(heuristic_factory),
            std::move(policy_filename),
            print_fact_names,
            report_epsilon,
            report_enabled,
            verbosity);
    }
};
} // namespace

namespace probfd::cli::solvers {

void add_ta_lrtdp_feature(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_declaration<TrialTerminationCondition>(
        {{"terminal", "Stop trials at terminal states"},
         {"consistent", "Stop trials at epsilon consistent states"},
         {"inconsistent", "Stop trials at epsilon inconsistent states"},
         {"revisited", "Stop trials upon revisiting a state"}});

    n.insert_function_definition<TrapAwareLRTDPSolverFeature>();
}

} // namespace probfd::cli::solvers
