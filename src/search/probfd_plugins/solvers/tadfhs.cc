#include "probfd_plugins/naming_conventions.h"

#include "probfd_plugins/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/open_list.h"
#include "probfd/algorithms/trap_aware_dfhs.h"
#include "probfd/solvers/mdp_heuristic_search.h"

#include "downward/plugins/plugin.h"

#include <memory>
#include <string>

using namespace plugins;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms;
using namespace probfd::algorithms::trap_aware_dfhs;

using namespace probfd_plugins;
using namespace probfd_plugins::solvers;

namespace {

using QOpenList = OpenList<quotients::QuotientAction<OperatorID>>;

class TrapAwareDFHSSolver : public MDPHeuristicSearch<false, true> {
    template <typename State, typename Action, bool Interval>
    using Algorithm = TADepthFirstHeuristicSearch<State, Action, Interval>;

    const std::shared_ptr<QOpenList> open_list_;
    const bool forward_updates_;
    const BacktrackingUpdateType backward_updates_;
    const bool cutoff_tip_;
    const bool cutoff_inconsistent_;
    const bool terminate_exploration_;
    const bool value_iteration_;
    const bool reexpand_traps_;

public:
    explicit TrapAwareDFHSSolver(
        std::shared_ptr<QOpenList> open_list,
        bool fwup,
        BacktrackingUpdateType bwup,
        bool cutoff_tip,
        bool cutoff_inconsistent,
        bool terminate_exploration,
        bool vi,
        bool reexpand_traps,
        bool fret_on_policy,
        bool dual_bounds,
        std::shared_ptr<PolicyPicker> policy,
        utils::Verbosity verbosity,
        const std::shared_ptr<TaskCostFunctionFactory>& costs,
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
              costs,
              std::move(path_dependent_evaluators),
              cache,
              eval,
              report_epsilon,
              report_enabled,
              max_time,
              std::move(policy_filename),
              print_fact_names)
        , open_list_(std::move(open_list))
        , forward_updates_(fwup)
        , backward_updates_(bwup)
        , cutoff_tip_(cutoff_tip)
        , cutoff_inconsistent_(cutoff_inconsistent)
        , terminate_exploration_(terminate_exploration)
        , value_iteration_(vi)
        , reexpand_traps_(reexpand_traps)
    {
    }

    std::string get_algorithm_name() const override { return "tadfhs"; }

    std::string get_heuristic_search_name() const override { return ""; }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        return this
            ->template create_quotient_heuristic_search_algorithm<Algorithm>(
                forward_updates_,
                backward_updates_,
                !cutoff_tip_,
                cutoff_inconsistent_,
                terminate_exploration_,
                value_iteration_,
                !value_iteration_,
                reexpand_traps_,
                open_list_);
    }
};

class TrapAwareDFHSSolverFeature
    : public TypedFeature<SolverInterface, TrapAwareDFHSSolver> {
public:
    TrapAwareDFHSSolverFeature()
        : TypedFeature<SolverInterface, TrapAwareDFHSSolver>("tadfhs")
    {
        document_title("Trap-aware depth-first heuristic search family.");
        document_synopsis(
            "Supports all MDPs (even non-SSPs) without FRET loop.");

        add_option<std::shared_ptr<QOpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            add_mdp_type_to_option<false, true>("lifo_open_list()"));

        add_option<bool>(
            "fwup",
            "Value updates on the way down of exploration.",
            "true");
        add_option<BacktrackingUpdateType>(
            "bwup",
            "Value updates on the way back of exploration",
            "ondemand");
        add_option<bool>(
            "cutoff_tip",
            "Do not follow tip states during policy exploration.",
            "true");
        add_option<bool>(
            "cutoff_inconsistent",
            "Do not expand states whose values have changed during the forward "
            "updates.",
            "true");
        add_option<bool>(
            "terminate_exploration",
            "Stop the exploration of the policy as soon as a state expansion "
            "was cutoff.",
            "false");
        add_option<bool>(
            "vi",
            "Run VI to check for termination. If set to false falling back to "
            "solved-labeling mechanism.",
            "false");
        add_option<bool>(
            "reexpand_traps",
            "Immediately re-expand the collapsed trap state.",
            "true");

        add_mdp_hs_options_to_feature<false, true>(*this);
    }

protected:
    std::shared_ptr<TrapAwareDFHSSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<TrapAwareDFHSSolver>(
            options.get<std::shared_ptr<QOpenList>>("open_list"),
            options.get<bool>("fwup"),
            options.get<BacktrackingUpdateType>("bwup"),
            options.get<bool>("cutoff_tip"),
            options.get<bool>("cutoff_inconsistent"),
            options.get<bool>("terminate_exploration"),
            options.get<bool>("vi"),
            options.get<bool>("reexpand_traps"),
            get_mdp_hs_args_from_options<false, true>(options));
    }
};

class TrapAwareILAOSolverFeature
    : public TypedFeature<SolverInterface, TrapAwareDFHSSolver> {
public:
    TrapAwareILAOSolverFeature()
        : TypedFeature<SolverInterface, TrapAwareDFHSSolver>("tailao")
    {
        document_title(
            "iLAO* variant of trap-aware depth-first heuristic search.");

        add_option<std::shared_ptr<QOpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            add_mdp_type_to_option<false, true>("lifo_open_list()"));

        add_mdp_hs_options_to_feature<false, true>(*this);
    }

    std::shared_ptr<TrapAwareDFHSSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        // opts_copy.set<std::string>("name", "ilao");
        // opts_copy.set<bool>("labeling", false);
        return plugins::make_shared_from_arg_tuples<TrapAwareDFHSSolver>(
            options.get<std::shared_ptr<QOpenList>>("open_list"),
            false,
            BacktrackingUpdateType::SINGLE,
            true,
            false,
            false,
            true,
            options.get<bool>("reexpand_traps"),
            get_mdp_hs_args_from_options<false, true>(options));
    }
};

class TrapAwareLILAOSolverFeature
    : public TypedFeature<SolverInterface, TrapAwareDFHSSolver> {
public:
    TrapAwareLILAOSolverFeature()
        : TypedFeature<SolverInterface, TrapAwareDFHSSolver>("talilao")
    {
        document_title("Labelled iLAO* variant of trap-aware depth-first "
                       "heuristic search.");

        add_option<std::shared_ptr<QOpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            add_mdp_type_to_option<false, true>("lifo_open_list()"));

        add_mdp_hs_options_to_feature<false, true>(*this);
    }

    std::shared_ptr<TrapAwareDFHSSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        // opts_copy.set<std::string>("name", "lilao");
        // opts_copy.set<bool>("labeling", true);

        return plugins::make_shared_from_arg_tuples<TrapAwareDFHSSolver>(
            options.get<std::shared_ptr<QOpenList>>("open_list"),
            false,
            BacktrackingUpdateType::SINGLE,
            true,
            false,
            false,
            false,
            options.get<bool>("reexpand_traps"),
            get_mdp_hs_args_from_options<false, true>(options));
    }
};

class TrapAwareHDPSolverFeature
    : public TypedFeature<SolverInterface, TrapAwareDFHSSolver> {
public:
    TrapAwareHDPSolverFeature()
        : TypedFeature<SolverInterface, TrapAwareDFHSSolver>("tahdp")
    {
        document_title(
            "HDP variant of trap-aware depth-first heuristic search.");

        add_option<std::shared_ptr<QOpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            add_mdp_type_to_option<false, true>("lifo_open_list()"));

        add_mdp_hs_options_to_feature<false, true>(*this);
    }

    std::shared_ptr<TrapAwareDFHSSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        // opts_copy.set<std::string>("name", "hdp");
        // opts_copy.set<bool>("labeling", true);
        return plugins::make_shared_from_arg_tuples<TrapAwareDFHSSolver>(
            options.get<std::shared_ptr<QOpenList>>("open_list"),
            true,
            BacktrackingUpdateType::ON_DEMAND,
            false,
            true,
            false,
            false,
            options.get<bool>("reexpand_traps"),
            get_mdp_hs_args_from_options<false, true>(options));
    }
};

FeaturePlugin<TrapAwareDFHSSolverFeature> _plugin;
FeaturePlugin<TrapAwareILAOSolverFeature> _plugin_ilao;
FeaturePlugin<TrapAwareLILAOSolverFeature> _plugin_lilao;
FeaturePlugin<TrapAwareHDPSolverFeature> _plugin_hdp;

TypedEnumPlugin<BacktrackingUpdateType> _fret_enum_plugin(
    {{"disabled", "No value updates are made."},
     {"ondemand", "update if some value changed during forward updates"},
     {"single", "single update"},
     {"convergence", "run value iteration until convergence"}});

} // namespace
