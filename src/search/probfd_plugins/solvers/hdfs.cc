#include "probfd_plugins/multi_feature_plugin.h"
#include "probfd_plugins/naming_conventions.h"

#include "probfd_plugins/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/heuristic_depth_first_search.h"

#include "downward/utils/system.h"

#include "downward/plugins/plugin.h"

#include <iostream>
#include <memory>
#include <string>

using namespace plugins;

using namespace probfd;
using namespace probfd::algorithms;
using namespace probfd::algorithms::heuristic_depth_first_search;
using namespace probfd::solvers;

using namespace probfd_plugins;
using namespace probfd_plugins::solvers;

namespace {

template <bool Bisimulation, bool Fret>
class DFHSSolver : public MDPHeuristicSearch<Bisimulation, Fret> {
    template <typename State, typename Action, bool Interval>
    using Algorithm = HeuristicDepthFirstSearch<State, Action, Interval>;

    const std::string name_;

    const bool labeling_;
    const bool forward_updates_;
    const BacktrackingUpdateType backward_updates_;
    const bool cutoff_inconsistent_;
    const bool partial_exploration_;
    const bool value_iteration_;
    const bool cutoff_tip_states_;

public:
    template <typename... Args>
    DFHSSolver(
        std::string variant_name,
        bool labeling,
        bool fwup,
        BacktrackingUpdateType bwup,
        bool cutoff_inconsistent,
        bool partial_exploration,
        bool vi,
        bool cutoff_tip,
        Args&&... args)
        : MDPHeuristicSearch<Bisimulation, Fret>(std::forward<Args>(args)...)
        , name_(std::move(variant_name))
        , labeling_(labeling)
        , forward_updates_(fwup)
        , backward_updates_(bwup)
        , cutoff_inconsistent_(cutoff_inconsistent)
        , partial_exploration_(partial_exploration)
        , value_iteration_(vi)
        , cutoff_tip_states_(cutoff_tip)
    {
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
            cutoff_tip_states_);
    }
};

template <bool Bisimulation, bool Fret>
class DFHSSolverFeature
    : public TypedFeature<SolverInterface, DFHSSolver<Bisimulation, Fret>> {
public:
    DFHSSolverFeature()
        : DFHSSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, Fret>("dfhs"))
    {
        this->document_title("Depth-first heuristic search family.");

        this->template add_option<bool>(
            "labeling",
            "",
            ArgumentInfo::NO_DEFAULT);
        this->template add_option<bool>("fwup", "", ArgumentInfo::NO_DEFAULT);
        this->template add_option<BacktrackingUpdateType>(
            "bwup",
            "",
            "ondemand");
        this->template add_option<bool>(
            "cutoff_inconsistent",
            "",
            ArgumentInfo::NO_DEFAULT);
        this->template add_option<bool>(
            "partial_exploration",
            "",
            ArgumentInfo::NO_DEFAULT);
        this->template add_option<bool>("vi", "", ArgumentInfo::NO_DEFAULT);
        this->template add_option<bool>(
            "cutoff_tip",
            "",
            ArgumentInfo::NO_DEFAULT);

        add_mdp_hs_options_to_feature<Bisimulation, Fret>(*this);
    }

protected:
    std::shared_ptr<DFHSSolver<Bisimulation, Fret>>
    create_component(const Options& options, const utils::Context& context)
        const override
    {
        using enum BacktrackingUpdateType;

        bool labeling = options.get<bool>("labeling");
        bool forward_updates = options.get<bool>("fwup");
        auto backward_updates = options.get<BacktrackingUpdateType>("bwup");
        bool cutoff_inconsistent = options.get<bool>("cutoff_inconsistent");
        bool partial_exploration = options.get<bool>("partial_exploration");
        bool value_iteration = options.get<bool>("vi");
        bool cutoff_tip = options.get<bool>("cutoff_tip");

        if (!forward_updates) {
            if (cutoff_inconsistent) {
                context.error("cutoff_inconsistent requires forward updates!");
            }
            if (backward_updates == ON_DEMAND) {
                if (cutoff_tip) {
                    context.error(
                        "ondemand backward updates require forward updates or "
                        "cutoff_tip=false!");
                }
            } else if (backward_updates == DISABLED && !value_iteration) {
                context.error("either value_iteration, forward_updates, or "
                              "backward_updates must be enabled!");
            }
        }

        if (partial_exploration && !cutoff_tip && !cutoff_inconsistent) {
            context.error(
                "greedy exploration requires either cutoff_tip=true or "
                "cutoff_inconsistent=true");
        }

        return plugins::make_shared_from_arg_tuples<
            DFHSSolver<Bisimulation, Fret>>(
            "dfhs",
            labeling,
            forward_updates,
            backward_updates,
            cutoff_inconsistent,
            partial_exploration,
            value_iteration,
            cutoff_tip,
            get_mdp_hs_args_from_options<Bisimulation, Fret>(options));
    }
};

template <bool Bisimulation, bool Fret>
class LAOSolverFeature
    : public TypedFeature<SolverInterface, DFHSSolver<Bisimulation, Fret>> {
public:
    LAOSolverFeature()
        : LAOSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, Fret>("lao"))
    {
        this->document_title("LAO* variant of depth-first heuristic search.");

        add_mdp_hs_options_to_feature<Bisimulation, Fret>(*this);
    }

    std::shared_ptr<DFHSSolver<Bisimulation, Fret>>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<
            DFHSSolver<Bisimulation, Fret>>(
            "lao",
            false,
            false,
            BacktrackingUpdateType::CONVERGENCE,
            true,
            false,
            false,
            false,
            get_mdp_hs_args_from_options<Bisimulation, Fret>(options));
    }
};

template <bool Bisimulation, bool Fret>
class ILAOSolverFeature
    : public TypedFeature<SolverInterface, DFHSSolver<Bisimulation, Fret>> {
public:
    ILAOSolverFeature()
        : ILAOSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, Fret>("ilao"))
    {
        this->document_title("iLAO* variant of depth-first heuristic search.");

        add_mdp_hs_options_to_feature<Bisimulation, Fret>(*this);
    }

    std::shared_ptr<DFHSSolver<Bisimulation, Fret>>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<
            DFHSSolver<Bisimulation, Fret>>(
            "ilao",
            false,
            false,
            BacktrackingUpdateType::SINGLE,
            false,
            false,
            true,
            true,
            get_mdp_hs_args_from_options<Bisimulation, Fret>(options));
    }
};

template <bool Bisimulation, bool Fret>
class HDPSolverFeature
    : public TypedFeature<SolverInterface, DFHSSolver<Bisimulation, Fret>> {
public:
    HDPSolverFeature()
        : HDPSolverFeature::TypedFeature(
              add_wrapper_algo_suffix<Bisimulation, Fret>("hdp"))
    {
        this->document_title("HDP variant of depth-first heuristic search.");

        add_mdp_hs_options_to_feature<Bisimulation, Fret>(*this);
    }

    std::shared_ptr<DFHSSolver<Bisimulation, Fret>>
    create_component(const Options& options, const utils::Context&)
        const override
    {
        return plugins::make_shared_from_arg_tuples<
            DFHSSolver<Bisimulation, Fret>>(
            "hdp",
            true,
            true,
            BacktrackingUpdateType::ON_DEMAND,
            true,
            false,
            false,
            false,
            get_mdp_hs_args_from_options<Bisimulation, Fret>(options));
    }
};

MultiFeaturePlugin<DFHSSolverFeature> _plugins_dfhs1;
MultiFeaturePlugin<LAOSolverFeature> _plugins_lao;
MultiFeaturePlugin<ILAOSolverFeature> _plugins_ilao;
MultiFeaturePlugin<HDPSolverFeature> _plugins_hdp;

TypedEnumPlugin<BacktrackingUpdateType> _fret_enum_plugin(
    {{"disabled",
      "No update is performed when backtracking from a state during the dfs "
      "exploration."},
     {"on_demand",
      "An update is performed when backtracking from a state during the dfs "
      "exploration, but only if a successor value was not epsilon-consistent "
      "during a recursive call."},
     {"single",
      "An update is always performed when backtracking from a state during the "
      "dfs exploration."},
     {"convergence",
      "Value iteration is performed until convergence when backtracking from "
      "an SCC."}});

} // namespace
