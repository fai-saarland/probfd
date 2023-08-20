#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/engines/heuristic_depth_first_search.h"

#include "downward/utils/system.h"

#include "downward/plugins/plugin.h"

#include <sstream>

namespace probfd {
namespace solvers {
namespace {

using namespace engines;
using namespace engines::heuristic_depth_first_search;

template <bool Bisimulation, bool Fret>
class DFHSSolver : public MDPHeuristicSearch<Bisimulation, Fret> {
    template <typename T>
    using WrappedType =
        typename MDPHeuristicSearch<Bisimulation, Fret>::template WrappedType<
            T>;

    template <typename State, typename Action, bool Interval>
    using Engine = HeuristicDepthFirstSearch<State, Action, Interval, Fret>;

    const std::string name_;

    const bool labeling_;
    const bool forward_updates_;
    const BacktrackingUpdateType backward_updates_;
    const bool cutoff_inconsistent_;
    const bool partial_exploration_;
    const bool value_iteration_;
    const bool expand_tip_states_;

public:
    explicit DFHSSolver(const plugins::Options& opts)
        : MDPHeuristicSearch<Bisimulation, Fret>(opts)
        , name_(opts.get<std::string>("name", "dfhs"))
        , labeling_(opts.get<bool>("labeling"))
        , forward_updates_(opts.get<bool>("fwup"))
        , backward_updates_(opts.get<BacktrackingUpdateType>("bwup"))
        , cutoff_inconsistent_(opts.get<bool>("cutoff_inconsistent"))
        , partial_exploration_(opts.get<bool>("partial_exploration"))
        , value_iteration_(opts.get<bool>("vi"))
        , expand_tip_states_(opts.get<bool>("expand_tip"))
    {
        parameter_validity_check();
    }

    std::string get_heuristic_search_name() const override { return name_; }

    std::unique_ptr<FDRMDPEngine> create_engine() override
    {
        return this->template create_heuristic_search_engine<Engine>(
            labeling_,
            forward_updates_,
            backward_updates_,
            cutoff_inconsistent_,
            partial_exploration_,
            value_iteration_,
            expand_tip_states_);
    }

    void parameter_validity_check() const
    {
        using enum BacktrackingUpdateType;

        bool valid = true;
        std::ostringstream error_msg;
        if (!forward_updates_) {
            if (cutoff_inconsistent_) {
                error_msg << "cutoff_inconsistent requires forward updates!"
                          << std::endl;
                valid = false;
            }
            if (backward_updates_ == ON_DEMAND) {
                error_msg
                    << "ondemand backward updates required forward updates!"
                    << std::endl;
                valid = false;
            }
            if (!value_iteration_ && backward_updates_ == DISABLED) {
                error_msg << "either value_iteration, forward_updates, or "
                             "backward_updates must be enabled!"
                          << std::endl;
                valid = false;
            }
            if (expand_tip_states_ && backward_updates_ == ON_DEMAND) {
                error_msg
                    << "ondemand backward updates require forward updates or "
                       "expand_tip=true!"
                    << std::endl;
                valid = false;
            }
        }
        if (partial_exploration_ && expand_tip_states_ &&
            !cutoff_inconsistent_) {
            error_msg
                << "greedy exploration requires either expand_tip=false or "
                   "cutoff_inconsistent=true"
                << std::endl;
            valid = false;
        }
        if (!valid) {
            std::cerr
                << "\nDepth-oriented heuristic search has been misconfigured!\n"
                << error_msg.str();
            utils::exit_with(utils::ExitCode::SEARCH_CRITICAL_ERROR);
        }
    }
};

class DFHSSolverFeature
    : public MDPFRETHeuristicSearchSolverFeature<DFHSSolver> {
public:
    DFHSSolverFeature()
        : MDPFRETHeuristicSearchSolverFeature<DFHSSolver>("dfhs")
    {
        document_title("Depth-first heuristic search family.");

        add_option<bool>("labeling", "", "true");
        add_option<bool>("fwup", "", "true");
        add_option<BacktrackingUpdateType>("bwup", "", "ondemand");
        add_option<bool>("cutoff_inconsistent", "", "true");
        add_option<bool>("partial_exploration", "", "false");
        add_option<bool>("vi", "", "false");
        add_option<bool>("expand_tip", "", "true");
    }
};

class LAOSolverFeature
    : public MDPFRETHeuristicSearchSolverFeature<DFHSSolver> {
public:
    LAOSolverFeature()
        : MDPFRETHeuristicSearchSolverFeature<DFHSSolver>("lao")
    {
        document_title("LAO* variant of depth-first heuristic search.");

        add_option<std::shared_ptr<FDROpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            plugins::ArgumentInfo::NO_DEFAULT);
    }

    std::shared_ptr<SolverInterface> create_component(
        const plugins::Options& options,
        const utils::Context& context) const override
    {
        plugins::Options opts_copy(options);
        opts_copy.set<std::string>("name", "ilao");
        opts_copy.set<bool>("labeling", false);
        opts_copy.set<bool>("fwup", false);
        opts_copy.set<BacktrackingUpdateType>(
            "bwup",
            BacktrackingUpdateType::CONVERGENCE);
        opts_copy.set<bool>("cutoff_inconsistent", true);
        opts_copy.set<bool>("partial_exploration", false);
        opts_copy.set<bool>("expand_tip", true);
        opts_copy.set<bool>("vi", false);
        return MDPFRETHeuristicSearchSolverFeature<
            DFHSSolver>::create_component(opts_copy, context);
    }
};

class ILAOSolverFeature
    : public MDPFRETHeuristicSearchSolverFeature<DFHSSolver> {
public:
    ILAOSolverFeature()
        : MDPFRETHeuristicSearchSolverFeature<DFHSSolver>("ilao")
    {
        document_title("iLAO* variant of depth-first heuristic search.");

        add_option<std::shared_ptr<FDROpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            plugins::ArgumentInfo::NO_DEFAULT);
    }

    std::shared_ptr<SolverInterface> create_component(
        const plugins::Options& options,
        const utils::Context& context) const override
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
        return MDPFRETHeuristicSearchSolverFeature<
            DFHSSolver>::create_component(opts_copy, context);
    }
};

class HDPSolverFeature
    : public MDPFRETHeuristicSearchSolverFeature<DFHSSolver> {
public:
    HDPSolverFeature()
        : MDPFRETHeuristicSearchSolverFeature<DFHSSolver>("hdp")
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

    std::shared_ptr<SolverInterface> create_component(
        const plugins::Options& options,
        const utils::Context& context) const override
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
        return MDPFRETHeuristicSearchSolverFeature<
            DFHSSolver>::create_component(opts_copy, context);
    }
};

static plugins::FeaturePlugin<DFHSSolverFeature> _plugin_dfhs;
static plugins::FeaturePlugin<LAOSolverFeature> _plugin_lao;
static plugins::FeaturePlugin<ILAOSolverFeature> _plugin_ilao;
static plugins::FeaturePlugin<HDPSolverFeature> _plugin_hdp;

static plugins::TypedEnumPlugin<BacktrackingUpdateType> _fret_enum_plugin(
    {{"disabled", ""}, {"ondemand", ""}, {"single", ""}, {"convergence", ""}});
}
} // namespace solvers
} // namespace probfd
