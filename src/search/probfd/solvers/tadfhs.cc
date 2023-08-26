#include "probfd/algorithms/open_list.h"
#include "probfd/algorithms/trap_aware_dfhs.h"
#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/plugins/naming_conventions.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace algorithms;
using namespace algorithms::trap_aware_dfhs;

using namespace plugins;

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
    explicit TrapAwareDFHSSolver(const Options& opts)
        : MDPHeuristicSearch<false, true>(opts)
        , open_list_(opts.get<std::shared_ptr<QOpenList>>("open_list"))
        , forward_updates_(opts.get<bool>("fwup"))
        , backward_updates_(opts.get<BacktrackingUpdateType>("bwup"))
        , cutoff_tip_(opts.get<bool>("cutoff_tip"))
        , cutoff_inconsistent_(opts.get<bool>("cutoff_inconsistent"))
        , terminate_exploration_(opts.get<bool>("terminate_exploration"))
        , value_iteration_(opts.get<bool>("vi"))
        , reexpand_traps_(opts.get<bool>("reexpand_traps"))
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

protected:
    void print_additional_statistics() const override
    {
        MDPHeuristicSearch<false, true>::print_additional_statistics();
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

        MDPHeuristicSearchBase::add_options_to_feature(*this);

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
        add_option<std::shared_ptr<QOpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            add_mdp_type_to_option<false, true>("lifo_open_list()"));
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

        MDPHeuristicSearchBase::add_options_to_feature(*this);

        add_option<std::shared_ptr<QOpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            add_mdp_type_to_option<false, true>("lifo_open_list()"));
    }

    std::shared_ptr<TrapAwareDFHSSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        Options opts_copy(options);
        opts_copy.set<std::string>("name", "ilao");
        opts_copy.set<bool>("labeling", false);
        opts_copy.set<bool>("fwup", false);
        opts_copy.set<BacktrackingUpdateType>(
            "bwup",
            BacktrackingUpdateType::SINGLE);
        opts_copy.set<bool>("cutoff_inconsistent", false);
        opts_copy.set<bool>("partial_exploration", false);
        opts_copy.set<bool>("expand_tip", false);
        opts_copy.set<bool>("vi", true);
        return std::make_shared<TrapAwareDFHSSolver>(opts_copy);
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

        MDPHeuristicSearchBase::add_options_to_feature(*this);

        add_option<std::shared_ptr<QOpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            add_mdp_type_to_option<false, true>("lifo_open_list()"));
    }

    std::shared_ptr<TrapAwareDFHSSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        Options opts_copy(options);
        opts_copy.set<std::string>("name", "lilao");
        opts_copy.set<bool>("labeling", true);
        opts_copy.set<bool>("fwup", false);
        opts_copy.set<BacktrackingUpdateType>(
            "bwup",
            BacktrackingUpdateType::SINGLE);
        opts_copy.set<bool>("cutoff_inconsistent", false);
        opts_copy.set<bool>("partial_exploration", false);
        opts_copy.set<bool>("expand_tip", false);
        opts_copy.set<bool>("vi", false);
        return std::make_shared<TrapAwareDFHSSolver>(opts_copy);
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

        MDPHeuristicSearchBase::add_options_to_feature(*this);

        add_option<std::shared_ptr<QOpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            add_mdp_type_to_option<false, true>("lifo_open_list()"));
    }

    std::shared_ptr<TrapAwareDFHSSolver>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        Options opts_copy(options);
        opts_copy.set<std::string>("name", "hdp");
        opts_copy.set<bool>("labeling", true);
        opts_copy.set<bool>("fwup", true);
        opts_copy.set<BacktrackingUpdateType>(
            "bwup",
            BacktrackingUpdateType::ON_DEMAND);
        opts_copy.set<bool>("cutoff_inconsistent", true);
        opts_copy.set<bool>("partial_exploration", false);
        opts_copy.set<bool>("vi", false);
        opts_copy.set<bool>("expand_tip", true);
        return std::make_shared<TrapAwareDFHSSolver>(opts_copy);
    }
};

static FeaturePlugin<TrapAwareDFHSSolverFeature> _plugin;
static FeaturePlugin<TrapAwareILAOSolverFeature> _plugin_ilao;
static FeaturePlugin<TrapAwareLILAOSolverFeature> _plugin_lilao;
static FeaturePlugin<TrapAwareHDPSolverFeature> _plugin_hdp;

static TypedEnumPlugin<BacktrackingUpdateType> _fret_enum_plugin(
    {{"disabled", "No value updates are made."},
     {"ondemand", "update if some value changed during forward updates"},
     {"single", "single update"},
     {"convergence", "run value iteration until convergence"}});
}
} // namespace solvers
} // namespace probfd
