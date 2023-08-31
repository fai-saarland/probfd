#include "probfd/algorithms/successor_sampler.h"
#include "probfd/algorithms/trap_aware_lrtdp.h"
#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/plugins/naming_conventions.h"

#include "downward/plugins/plugin.h"

#include <memory>

namespace probfd {
namespace solvers {
namespace {

using namespace algorithms;
using namespace algorithms::trap_aware_lrtdp;
using namespace plugins;

using QSuccessorSampler =
    SuccessorSampler<quotients::QuotientAction<OperatorID>>;

class TrapAwareLRTDPSolver : public MDPHeuristicSearch<false, true> {
    const TrialTerminationCondition stop_consistent_;
    const bool reexpand_traps_;
    const std::shared_ptr<QSuccessorSampler> successor_sampler_;

public:
    explicit TrapAwareLRTDPSolver(const Options& opts)
        : MDPHeuristicSearch<false, true>(opts)
        , stop_consistent_(
              opts.get<TrialTerminationCondition>("terminate_trial"))
        , reexpand_traps_(opts.get<bool>("reexpand_traps"))
        , successor_sampler_(
              opts.get<std::shared_ptr<QSuccessorSampler>>("successor_sampler"))
    {
    }

    std::string get_algorithm_name() const override { return "talrtdp"; }
    std::string get_heuristic_search_name() const override { return ""; }

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        return this
            ->template create_quotient_heuristic_search_algorithm<TALRTDP>(
                stop_consistent_,
                reexpand_traps_,
                successor_sampler_);
    }

protected:
    void print_additional_statistics() const override
    {
        MDPHeuristicSearch<false, true>::print_additional_statistics();
        successor_sampler_->print_statistics(std::cout);
    }
};

class TrapAwareLRTDPSolverFeature
    : public TypedFeature<SolverInterface, TrapAwareLRTDPSolver> {
public:
    TrapAwareLRTDPSolverFeature()
        : TypedFeature<SolverInterface, TrapAwareLRTDPSolver>("talrtdp")
    {
        document_title("Trap-aware LRTDP.");
        document_synopsis(
            "Supports all MDP types (even non-SSPs) without FRET loop.");

        MDPHeuristicSearch<false, true>::add_options_to_feature(*this);

        add_option<std::shared_ptr<QSuccessorSampler>>(
            "successor_sampler",
            "Successor bias for the trials.",
            add_mdp_type_to_option<false, true>("random_successor_sampler"));
        add_option<TrialTerminationCondition>(
            "terminate_trial",
            "The trial termination condition.",
            "consistent");
        add_option<bool>(
            "reexpand_traps",
            "Immediately re-expand the collapsed trap state.",
            "true");
    }
};

static FeaturePlugin<TrapAwareLRTDPSolverFeature> _plugin;

static TypedEnumPlugin<TrialTerminationCondition> _enum_plugin(
    {{"terminal", "Stop trials at terminal states"},
     {"consistent", "Stop trials at epsilon consistent states"},
     {"inconsistent", "Stop trials at epsilon inconsistent states"},
     {"revisited", "Stop trials upon revisiting a state"}});

} // namespace
} // namespace solvers
} // namespace probfd
