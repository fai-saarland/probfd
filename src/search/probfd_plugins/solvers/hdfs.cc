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
    const bool expand_tip_states_;

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
        bool expand_tip,
        Args&&... args)
        : MDPHeuristicSearch<Bisimulation, Fret>(std::forward<Args>(args)...)
        , name_(std::move(variant_name))
        , labeling_(labeling)
        , forward_updates_(fwup)
        , backward_updates_(bwup)
        , cutoff_inconsistent_(cutoff_inconsistent)
        , partial_exploration_(partial_exploration)
        , value_iteration_(vi)
        , expand_tip_states_(expand_tip)
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
            expand_tip_states_);
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
        bool expand_tip = options.get<bool>("expand_tip");

        if (!forward_updates) {
            if (cutoff_inconsistent) {
                context.error("cutoff_inconsistent requires forward updates!");
            }
            if (backward_updates == ON_DEMAND) {
                context.error("ondemand backward updates required forward "
                              "updates!");
            }
            if (!value_iteration && backward_updates == DISABLED) {
                context.error("either value_iteration, forward_updates, or "
                              "backward_updates must be enabled!");
            }
            if (expand_tip && backward_updates == ON_DEMAND) {
                context.error(
                    "ondemand backward updates require forward updates or "
                    "expand_tip=true!");
            }
        }

        if (partial_exploration && expand_tip && !cutoff_inconsistent) {
            context.error(
                "greedy exploration requires either expand_tip=false or "
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
            expand_tip,
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
            true,
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
            false,
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
        this->document_title(
            "HDP variant of trap-aware depth-first heuristic search.");

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
            true,
            get_mdp_hs_args_from_options<Bisimulation, Fret>(options));
    }
};

MultiFeaturePlugin<DFHSSolverFeature> _plugins_dfhs1;
MultiFeaturePlugin<LAOSolverFeature> _plugins_lao;
MultiFeaturePlugin<ILAOSolverFeature> _plugins_ilao;
MultiFeaturePlugin<HDPSolverFeature> _plugins_hdp;

TypedEnumPlugin<BacktrackingUpdateType> _fret_enum_plugin(
    {{"disabled", ""}, {"ondemand", ""}, {"single", ""}, {"convergence", ""}});

} // namespace
