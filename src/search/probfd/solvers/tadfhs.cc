#include "probfd/engine_interfaces/open_list.h"
#include "probfd/engines/trap_aware_dfhs.h"
#include "probfd/solvers/mdp_heuristic_search.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace engine_interfaces;
using namespace engines::trap_aware_dfhs;

class TrapAwareDFHSSolver : public MDPHeuristicSearch<false, true> {
    template <typename T>
    using WrappedType =
        typename MDPHeuristicSearch<false, true>::WrappedType<T>;

    template <typename State, typename Action, bool Interval>
    using Engine = TADepthFirstHeuristicSearch<State, Action, Interval>;

    WrappedType<std::shared_ptr<FDROpenList>> open_list_;
    const bool forward_updates_;
    const BacktrackingUpdateType backward_updates_;
    const bool cutoff_tip_;
    const bool cutoff_inconsistent_;
    const bool terminate_exploration_;
    const bool value_iteration_;
    const bool reexpand_traps_;

public:
    explicit TrapAwareDFHSSolver(const plugins::Options& opts)
        : MDPHeuristicSearch<false, true>(opts)
        , open_list_(
              opts.contains("open_list")
                  ? this->wrap(
                        opts.get<std::shared_ptr<FDROpenList>>("open_list"))
                  : nullptr)
        , forward_updates_(opts.get<bool>("fwup"))
        , backward_updates_(opts.get<BacktrackingUpdateType>("bwup"))
        , cutoff_tip_(opts.get<bool>("cutoff_tip"))
        , cutoff_inconsistent_(opts.get<bool>("cutoff_inconsistent"))
        , terminate_exploration_(opts.get<bool>("terminate_exploration"))
        , value_iteration_(opts.get<bool>("vi"))
        , reexpand_traps_(opts.get<bool>("reexpand_traps"))
    {
    }

    std::string get_engine_name() const override { return "tadfhs"; }

    std::string get_heuristic_search_name() const override { return ""; }

    std::unique_ptr<FDRMDPEngine> create_engine() override
    {
        return this->template create_quotient_heuristic_search_engine<Engine>(
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
    : public plugins::TypedFeature<SolverInterface, TrapAwareDFHSSolver> {
public:
    TrapAwareDFHSSolverFeature()
        : plugins::TypedFeature<SolverInterface, TrapAwareDFHSSolver>("tadfhs")
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
        add_option<std::shared_ptr<FDROpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            plugins::ArgumentInfo::NO_DEFAULT);
    }
};

class TrapAwareILAOSolverFeature
    : public plugins::TypedFeature<SolverInterface, TrapAwareDFHSSolver> {
public:
    TrapAwareILAOSolverFeature()
        : plugins::TypedFeature<SolverInterface, TrapAwareDFHSSolver>("tailao")
    {
        document_title(
            "iLAO* variant of trap-aware depth-first heuristic search.");

        MDPHeuristicSearchBase::add_options_to_feature(*this);

        add_option<std::shared_ptr<FDROpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            plugins::ArgumentInfo::NO_DEFAULT);
    }

    std::shared_ptr<TrapAwareDFHSSolver>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        plugins::Options opts_copy(options);
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
    : public plugins::TypedFeature<SolverInterface, TrapAwareDFHSSolver> {
public:
    TrapAwareLILAOSolverFeature()
        : plugins::TypedFeature<SolverInterface, TrapAwareDFHSSolver>("talilao")
    {
        document_title("Labelled iLAO* variant of trap-aware depth-first "
                       "heuristic search.");

        MDPHeuristicSearchBase::add_options_to_feature(*this);

        add_option<std::shared_ptr<FDROpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            plugins::ArgumentInfo::NO_DEFAULT);
    }

    std::shared_ptr<TrapAwareDFHSSolver>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        plugins::Options opts_copy(options);
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
    : public plugins::TypedFeature<SolverInterface, TrapAwareDFHSSolver> {
public:
    TrapAwareHDPSolverFeature()
        : plugins::TypedFeature<SolverInterface, TrapAwareDFHSSolver>("tahdp")
    {
        document_title(
            "HDP variant of trap-aware depth-first heuristic search.");

        MDPHeuristicSearchBase::add_options_to_feature(*this);

        add_option<std::shared_ptr<FDROpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            plugins::ArgumentInfo::NO_DEFAULT);
    }

    std::shared_ptr<TrapAwareDFHSSolver>
    create_component(const plugins::Options& options, const utils::Context&)
        const override
    {
        plugins::Options opts_copy(options);
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

static plugins::FeaturePlugin<TrapAwareDFHSSolverFeature> _plugin;
static plugins::FeaturePlugin<TrapAwareILAOSolverFeature> _plugin_ilao;
static plugins::FeaturePlugin<TrapAwareLILAOSolverFeature> _plugin_lilao;
static plugins::FeaturePlugin<TrapAwareHDPSolverFeature> _plugin_hdp;

static plugins::TypedEnumPlugin<BacktrackingUpdateType> _fret_enum_plugin(
    {{"disabled", "No value updates are made."},
     {"ondemand", "update if some value changed during forward updates"},
     {"single", "single update"},
     {"convergence", "run value iteration until convergence"}});
}
} // namespace solvers
} // namespace probfd
