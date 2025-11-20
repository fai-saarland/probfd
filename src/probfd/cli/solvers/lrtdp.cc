#include "probfd/cli/solvers/lrtdp.h"

#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search_options.h"
#include "probfd/cli/solvers/mdp_solver_options.h"

#include "probfd/algorithms/lrtdp.h"
#include "probfd/solvers/mdp_heuristic_search.h"

#include "downward/cli/plugins/registry.h"

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

template <bool Bisimulation>
class LRTDPSolverFeature
    : public SharedTypedFeature<
          TaskSolverFactory,
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          Verbosity,
          value_t,
          bool,
          std::shared_ptr<PolicyPickerType<Bisimulation, false>>,
          std::shared_ptr<SuccessorSampler<ActionType<Bisimulation, false>>>,
          TrialTerminationCondition> {
    using Sampler = SuccessorSampler<ActionType<Bisimulation, false>>;

public:
    LRTDPSolverFeature()
        : LRTDPSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, false>("lrtdp"),
              &LRTDPSolverFeature::func)
    {
        this->document_title("Labelled Real-Time Dynamic Programming");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);
        const auto n2 =
            add_mdp_hs_options_to_feature<Bisimulation, false>(*this, n);

        this->make_optional_argument_with_default(
            n + n2,
            "successor_sampler",
            add_mdp_type_to_option<Bisimulation, false>(
                "random_successor_sampler()"));

        this->make_optional_argument_with_default(
            n + n2 + 1,
            "trial_termination",
            "terminal");
    }

protected:
    static std::shared_ptr<TaskSolverFactory> func(
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        Verbosity verbosity,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPickerType<Bisimulation, false>> policy,
        std::shared_ptr<Sampler> successor_sampler,
        TrialTerminationCondition trial_termination)
    {
        using enum TrialTerminationCondition;

        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<LRTDPSolver<Bisimulation, false>>(
                std::move(successor_sampler),
                std::move(trial_termination),
                convergence_epsilon,
                dual_bounds,
                std::move(policy)),
            std::move(task_state_space_factory),
            std::move(heuristic_factory),
            std::move(policy_filename),
            print_fact_names,
            report_epsilon,
            report_enabled,
            verbosity);
    }
};

template <bool Bisimulation>
class LRTDPFretSolverFeature
    : public SharedTypedFeatureWithContext<
          TaskSolverFactory,
          std::shared_ptr<TaskStateSpaceFactory>,
          std::shared_ptr<TaskHeuristicFactory>,
          std::string,
          bool,
          value_t,
          bool,
          Verbosity,
          bool,
          value_t,
          bool,
          std::shared_ptr<PolicyPickerType<Bisimulation, true>>,
          std::shared_ptr<SuccessorSampler<ActionType<Bisimulation, true>>>,
          TrialTerminationCondition> {
    using Sampler = SuccessorSampler<ActionType<Bisimulation, true>>;

public:
    LRTDPFretSolverFeature()
        : LRTDPFretSolverFeature::TypedFeatureWithContext(
              add_wrapper_algo_suffix<Bisimulation, true>("lrtdp"),
              &LRTDPFretSolverFeature::func)
    {
        this->document_title("Labelled Real-Time Dynamic Programming");

        const auto n =
            add_base_solver_options_except_algorithm_to_feature(*this, 0);
        const auto n2 =
            add_mdp_hs_options_to_feature<Bisimulation, true>(*this, n);

        this->make_optional_argument_with_default(
            n + n2,
            "successor_sampler",
            add_mdp_type_to_option<Bisimulation, true>(
                "random_successor_sampler()"));

        this->make_optional_argument_with_default(
            n + n2 + 1,
            "trial_termination",
            "terminal");
    }

protected:
    static std::shared_ptr<TaskSolverFactory> func(
        const Context& context,
        std::shared_ptr<TaskStateSpaceFactory> task_state_space_factory,
        std::shared_ptr<TaskHeuristicFactory> heuristic_factory,
        std::string policy_filename,
        bool print_fact_names,
        value_t report_epsilon,
        bool report_enabled,
        Verbosity verbosity,
        bool fret_on_policy,
        value_t convergence_epsilon,
        bool dual_bounds,
        std::shared_ptr<PolicyPickerType<Bisimulation, true>> policy,
        std::shared_ptr<Sampler> successor_sampler,
        TrialTerminationCondition trial_termination)
    {
        using enum TrialTerminationCondition;

        if (trial_termination != CONSISTENT && trial_termination != REVISITED) {
            context.warn(
                "Warning: LRTDP is run within FRET with an unsafe "
                "trial termination condition! LRTDP's trials may "
                "get stuck in cycles.");
        }

        return make_shared_from_arg_tuples<MDPSolver>(
            make_shared_from_arg_tuples<LRTDPSolver<Bisimulation, true>>(
                std::move(successor_sampler),
                std::move(trial_termination),
                fret_on_policy,
                convergence_epsilon,
                dual_bounds,
                std::move(policy)),
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

void add_lrtdp_features(Registry& registry)
{
    Namespace& n = registry.get_global_name_space();
    n.insert_enum_plugin<TrialTerminationCondition>(
        {{"terminal", "Stop trials at terminal states"},
         {"consistent", "Stop trials at epsilon consistent states"},
         {"inconsistent", "Stop trials at epsilon inconsistent states"},
         {"revisited", "Stop trials upon revisiting a state"}});

    n.insert_feature_plugins<LRTDPSolverFeature>();
    n.insert_feature_plugins<LRTDPFretSolverFeature>();
}

} // namespace probfd::cli::solvers
