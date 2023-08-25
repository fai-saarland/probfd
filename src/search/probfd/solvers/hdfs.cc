#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/heuristic_depth_first_search.h"

#include "probfd/plugins/multi_feature_plugin.h"
#include "probfd/plugins/naming_conventions.h"

#include "downward/utils/system.h"

#include "downward/plugins/plugin.h"

#include <sstream>

namespace probfd {
namespace solvers {
namespace {

using namespace algorithms;
using namespace algorithms::heuristic_depth_first_search;

using namespace plugins;

template <bool Bisimulation, bool Fret>
class DFHSSolver : public MDPHeuristicSearch<Bisimulation, Fret> {
    template <typename State, typename Action, bool Interval>
    using Algorithm = HeuristicDepthFirstSearch<State, Action, Interval, Fret>;

    const std::string name_;

    const bool labeling_;
    const bool forward_updates_;
    const BacktrackingUpdateType backward_updates_;
    const bool cutoff_inconsistent_;
    const bool partial_exploration_;
    const bool value_iteration_;
    const bool expand_tip_states_;

public:
    explicit DFHSSolver(const Options& opts)
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

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        return this->template create_heuristic_search_algorithm<Algorithm>(
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

template <bool Bisimulation, bool Fret>
class DFHSSolverFeature
    : public MDPFRETHeuristicSearchSolverFeature<
          DFHSSolver,
          Bisimulation,
          Fret> {
public:
    DFHSSolverFeature()
        : MDPFRETHeuristicSearchSolverFeature<DFHSSolver, Bisimulation, Fret>(
              add_mdp_type_to_option<Bisimulation, Fret>("dfhs"))
    {
        this->document_title("Depth-first heuristic search family.");

        this->template add_option<bool>("labeling", "", "true");
        this->template add_option<bool>("fwup", "", "true");
        this->template add_option<BacktrackingUpdateType>(
            "bwup",
            "",
            "ondemand");
        this->template add_option<bool>("cutoff_inconsistent", "", "true");
        this->template add_option<bool>("partial_exploration", "", "false");
        this->template add_option<bool>("vi", "", "false");
        this->template add_option<bool>("expand_tip", "", "true");
    }
};

template <bool Bisimulation, bool Fret>
class LAOSolverFeature
    : public MDPFRETHeuristicSearchSolverFeature<
          DFHSSolver,
          Bisimulation,
          Fret> {
public:
    LAOSolverFeature()
        : MDPFRETHeuristicSearchSolverFeature<DFHSSolver, Bisimulation, Fret>(
              add_mdp_type_to_option<Bisimulation, Fret>("lao"))
    {
        this->document_title("LAO* variant of depth-first heuristic search.");

        this->template add_option<std::shared_ptr<FDROpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            ArgumentInfo::NO_DEFAULT);
    }

    std::shared_ptr<DFHSSolver<Bisimulation, Fret>>
    create_component(const Options& options, const utils::Context& context)
        const override
    {
        Options opts_copy(options);
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
            DFHSSolver,
            Bisimulation,
            Fret>::create_component(opts_copy, context);
    }
};

template <bool Bisimulation, bool Fret>
class ILAOSolverFeature
    : public MDPFRETHeuristicSearchSolverFeature<
          DFHSSolver,
          Bisimulation,
          Fret> {
public:
    ILAOSolverFeature()
        : MDPFRETHeuristicSearchSolverFeature<DFHSSolver, Bisimulation, Fret>(
              add_mdp_type_to_option<Bisimulation, Fret>("ilao"))
    {
        this->document_title("iLAO* variant of depth-first heuristic search.");

        this->template add_option<std::shared_ptr<FDROpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            ArgumentInfo::NO_DEFAULT);
    }

    std::shared_ptr<DFHSSolver<Bisimulation, Fret>>
    create_component(const Options& options, const utils::Context& context)
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
        return MDPFRETHeuristicSearchSolverFeature<
            DFHSSolver,
            Bisimulation,
            Fret>::create_component(opts_copy, context);
    }
};

template <bool Bisimulation, bool Fret>
class HDPSolverFeature
    : public MDPFRETHeuristicSearchSolverFeature<
          DFHSSolver,
          Bisimulation,
          Fret> {
public:
    HDPSolverFeature()
        : MDPFRETHeuristicSearchSolverFeature<DFHSSolver, Bisimulation, Fret>(
              add_mdp_type_to_option<Bisimulation, Fret>("hdp"))
    {
        this->document_title(
            "HDP variant of trap-aware depth-first heuristic search.");

        this->template add_option<std::shared_ptr<FDROpenList>>(
            "open_list",
            "Ordering in which successors are considered during policy "
            "exploration.",
            ArgumentInfo::NO_DEFAULT);
    }

    std::shared_ptr<DFHSSolver<Bisimulation, Fret>>
    create_component(const Options& options, const utils::Context& context)
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
        return MDPFRETHeuristicSearchSolverFeature<
            DFHSSolver,
            Bisimulation,
            Fret>::create_component(opts_copy, context);
    }
};

static MultiFeaturePlugin<DFHSSolverFeature> _plugins_dfhs1;
static MultiFeaturePlugin<LAOSolverFeature> _plugins_lao;
static MultiFeaturePlugin<ILAOSolverFeature> _plugins_ilao;
static MultiFeaturePlugin<HDPSolverFeature> _plugins_hdp;

static TypedEnumPlugin<BacktrackingUpdateType> _fret_enum_plugin(
    {{"disabled", ""}, {"ondemand", ""}, {"single", ""}, {"convergence", ""}});
}
} // namespace solvers
} // namespace probfd
