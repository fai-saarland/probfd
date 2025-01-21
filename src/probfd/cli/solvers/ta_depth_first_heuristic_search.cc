#include "downward/cli/plugins/plugin.h"

#include "probfd/cli/naming_conventions.h"

#include "probfd/cli/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/open_list.h"
#include "probfd/algorithms/trap_aware_dfhs.h"
#include "probfd/solvers/mdp_heuristic_search.h"

#include <memory>
#include <string>

using namespace utils;

using namespace probfd;
using namespace probfd::solvers;
using namespace probfd::algorithms;
using namespace probfd::algorithms::trap_aware_dfhs;

using namespace probfd::cli;
using namespace probfd::cli::solvers;

using namespace downward::cli::plugins;

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
    const bool labeling_;
    const bool reexpand_traps_;

public:
    template <typename... Args>
    explicit TrapAwareDFHSSolver(
        std::shared_ptr<QOpenList> open_list,
        bool fwup,
        BacktrackingUpdateType bwup,
        bool cutoff_tip,
        bool cutoff_inconsistent,
        bool terminate_exploration,
        bool labeling,
        bool reexpand_traps,
        Args&&... args)
        : MDPHeuristicSearch<false, true>(std::forward<Args>(args)...)
        , open_list_(std::move(open_list))
        , forward_updates_(fwup)
        , backward_updates_(bwup)
        , cutoff_tip_(cutoff_tip)
        , cutoff_inconsistent_(cutoff_inconsistent)
        , terminate_exploration_(terminate_exploration)
        , labeling_(labeling)
        , reexpand_traps_(reexpand_traps)
    {
    }

    std::string get_algorithm_name() const override { return "tadfhs"; }

    std::string get_heuristic_search_name() const override { return ""; }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm(
        const std::shared_ptr<ProbabilisticTask>& task,
        const std::shared_ptr<FDRCostFunction>& task_cost_function) override
    {
        return this
            ->template create_quotient_heuristic_search_algorithm<Algorithm>(
                task,
                task_cost_function,
                forward_updates_,
                backward_updates_,
                cutoff_tip_,
                cutoff_inconsistent_,
                terminate_exploration_,
                labeling_,
                reexpand_traps_);
    }
};

class TrapAwareDFHSSolverFeature
    : public TypedFeature<TaskSolverFactory, TrapAwareDFHSSolver> {
public:
    TrapAwareDFHSSolverFeature()
        : TypedFeature<TaskSolverFactory, TrapAwareDFHSSolver>("tadfhs")
    {
        document_title("Trap-aware depth-first heuristic search family");
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
        add_option<bool>("labeling", "Label states as solved.", "true");
        add_option<bool>(
            "reexpand_traps",
            "Immediately re-expand the collapsed trap state.",
            "true");

        add_mdp_hs_options_to_feature<false, true>(*this);
    }

protected:
    std::shared_ptr<TrapAwareDFHSSolver>
    create_component(const Options& options, const Context&) const override
    {
        return make_shared_from_arg_tuples<TrapAwareDFHSSolver>(
            options.get<std::shared_ptr<QOpenList>>("open_list"),
            options.get<bool>("fwup"),
            options.get<BacktrackingUpdateType>("bwup"),
            options.get<bool>("cutoff_tip"),
            options.get<bool>("cutoff_inconsistent"),
            options.get<bool>("terminate_exploration"),
            options.get<bool>("labeling"),
            options.get<bool>("reexpand_traps"),
            get_mdp_hs_args_from_options<false, true>(options));
    }
};

class TrapAwareILAOSolverFeature
    : public TypedFeature<TaskSolverFactory, TrapAwareDFHSSolver> {
public:
    TrapAwareILAOSolverFeature()
        : TypedFeature<TaskSolverFactory, TrapAwareDFHSSolver>("tailao")
    {
        document_title(
            "iLAO* variant of trap-aware depth-first heuristic search");

        add_option<std::shared_ptr<QOpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            add_mdp_type_to_option<false, true>("lifo_open_list()"));
        add_option<bool>(
            "reexpand_traps",
            "Immediately re-expand the collapsed trap state.",
            "true");

        add_mdp_hs_options_to_feature<false, true>(*this);
    }

    std::shared_ptr<TrapAwareDFHSSolver>
    create_component(const Options& options, const Context&) const override
    {
        // opts_copy.set<std::string>("name", "ilao");
        return make_shared_from_arg_tuples<TrapAwareDFHSSolver>(
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

class TrapAwareLILAOSolverFeature
    : public TypedFeature<TaskSolverFactory, TrapAwareDFHSSolver> {
public:
    TrapAwareLILAOSolverFeature()
        : TypedFeature<TaskSolverFactory, TrapAwareDFHSSolver>("talilao")
    {
        document_title("Labelled iLAO* variant of trap-aware depth-first "
                       "heuristic search");

        add_option<std::shared_ptr<QOpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            add_mdp_type_to_option<false, true>("lifo_open_list()"));
        add_option<bool>(
            "reexpand_traps",
            "Immediately re-expand the collapsed trap state.",
            "true");

        add_mdp_hs_options_to_feature<false, true>(*this);
    }

    std::shared_ptr<TrapAwareDFHSSolver>
    create_component(const Options& options, const Context&) const override
    {
        // opts_copy.set<std::string>("name", "lilao");
        // opts_copy.set<bool>("labeling", true);

        return make_shared_from_arg_tuples<TrapAwareDFHSSolver>(
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

class TrapAwareHDPSolverFeature
    : public TypedFeature<TaskSolverFactory, TrapAwareDFHSSolver> {
public:
    TrapAwareHDPSolverFeature()
        : TypedFeature<TaskSolverFactory, TrapAwareDFHSSolver>("tahdp")
    {
        document_title(
            "HDP variant of trap-aware depth-first heuristic search");

        add_option<std::shared_ptr<QOpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            add_mdp_type_to_option<false, true>("lifo_open_list()"));
        add_option<bool>(
            "reexpand_traps",
            "Immediately re-expand the collapsed trap state.",
            "true");

        add_mdp_hs_options_to_feature<false, true>(*this);
    }

    std::shared_ptr<TrapAwareDFHSSolver>
    create_component(const Options& options, const Context&) const override
    {
        // opts_copy.set<std::string>("name", "hdp");
        // opts_copy.set<bool>("labeling", true);
        return make_shared_from_arg_tuples<TrapAwareDFHSSolver>(
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
     {"single", "single update"}});

} // namespace
