#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/engines/heuristic_depth_first_search.h"

#include "utils/system.h"

#include "option_parser.h"
#include "plugin.h"

#include <sstream>

namespace probfd {
namespace solvers {
namespace {

using namespace engine_interfaces;
using namespace engines::heuristic_depth_first_search;

template <bool Bisimulation, bool Fret>
class HDFSSolver : public MDPHeuristicSearch<Bisimulation, Fret> {
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
    explicit HDFSSolver(const options::Options& opts)
        : MDPHeuristicSearch<Bisimulation, Fret>(opts)
        , name_(opts.contains("name") ? opts.get<std::string>("name") : "hdfs")
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

    virtual std::string get_heuristic_search_name() const override
    {
        return name_;
    }

    virtual engines::MDPEngineInterface<State, OperatorID>*
    create_engine() override
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

struct HDFSOptions {
    void operator()(options::OptionParser& parser) const
    {
        MDPHeuristicSearchBase::add_options_to_parser(parser);
        parser.add_option<bool>("labeling", "", "true");
        parser.add_option<bool>("fwup", "", "true");
        {
            std::vector<std::string> bwups(
                {"disabled", "ondemand", "single", "convergence"});
            parser.add_enum_option<BacktrackingUpdateType>(
                "bwup",
                bwups,
                "",
                "ondemand");
        }
        parser.add_option<bool>("cutoff_inconsistent", "", "true");
        parser.add_option<bool>("partial_exploration", "", "false");
        parser.add_option<bool>("vi", "", "false");
        parser.add_option<bool>("expand_tip", "", "true");
    }
};

struct LAOOptions {
    void operator()(options::Options& opts) const
    {
        opts.set<std::string>("name", "lao");
        opts.set<bool>("labeling", false);
        opts.set<bool>("fwup", false);
        opts.set<BacktrackingUpdateType>(
            "bwup",
            BacktrackingUpdateType::CONVERGENCE);
        opts.set<bool>("cutoff_inconsistent", false);
        opts.set<bool>("partial_exploration", true);
        opts.set<bool>("expand_tip", false);
        opts.set<bool>("vi", true);
    }
};

struct ILAOOptions {
    void operator()(options::Options& opts) const
    {
        opts.set<std::string>("name", "ilao");
        opts.set<bool>("labeling", false);
        opts.set<bool>("fwup", false);
        opts.set<BacktrackingUpdateType>(
            "bwup",
            BacktrackingUpdateType::SINGLE);
        opts.set<bool>("cutoff_inconsistent", false);
        opts.set<bool>("partial_exploration", false);
        opts.set<bool>("expand_tip", false);
        opts.set<bool>("vi", true);
    }
};

struct HDPOptions {
    void operator()(options::Options& opts) const
    {
        opts.set<std::string>("name", "hdp");
        opts.set<bool>("labeling", true);
        opts.set<bool>("fwup", true);
        opts.set<BacktrackingUpdateType>(
            "bwup",
            BacktrackingUpdateType::ON_DEMAND);
        opts.set<bool>("cutoff_inconsistent", true);
        opts.set<bool>("partial_exploration", false);
        opts.set<bool>("vi", false);
        opts.set<bool>("expand_tip", true);
    }
};

static Plugin<SolverInterface> _plugin0(
    "hdfs",
    parse_mdp_heuristic_search_solver<HDFSSolver, HDFSOptions>);

static Plugin<SolverInterface> _plugin1(
    "lao",
    parse_mdp_heuristic_search_solver<
        HDFSSolver,
        NoAdditionalOptions,
        LAOOptions>);

static Plugin<SolverInterface> _plugin2(
    "ilao",
    parse_mdp_heuristic_search_solver<
        HDFSSolver,
        NoAdditionalOptions,
        ILAOOptions>);

static Plugin<SolverInterface> _plugin4(
    "hdp",
    parse_mdp_heuristic_search_solver<
        HDFSSolver,
        NoAdditionalOptions,
        HDPOptions>);

}
} // namespace solvers
} // namespace probfd
