#include "probfd/engine_interfaces/successor_sampler.h"
#include "probfd/engines/trap_aware_lrtdp.h"
#include "probfd/solvers/mdp_heuristic_search.h"
#include "probfd/successor_samplers/task_successor_sampler_factory.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace engine_interfaces;
using namespace engines::trap_aware_lrtdp;

class TrapAwareLRTDPSolver : public MDPHeuristicSearch<false, true> {
    template <typename T>
    using WrappedType =
        typename MDPHeuristicSearch<false, true>::WrappedType<T>;

    template <typename State, typename Action, bool Interval>
    using Engine = TALRTDP<State, Action, Interval>;

    const TrialTerminationCondition stop_consistent_;
    const bool reexpand_traps_;
    WrappedType<std::shared_ptr<TaskSuccessorSampler>> successor_sampler_;

public:
    explicit TrapAwareLRTDPSolver(const plugins::Options& opts)
        : MDPHeuristicSearch<false, true>(opts)
        , stop_consistent_(
              opts.get<TrialTerminationCondition>("terminate_trial"))
        , reexpand_traps_(opts.get<bool>("reexpand_traps"))
        , successor_sampler_(
              this->wrap(opts.get<std::shared_ptr<TaskSuccessorSamplerFactory>>(
                                 "successor_sampler")
                             ->create_sampler(this->state_space_.get())))
    {
    }

    std::string get_engine_name() const override { return "talrtdp"; }
    std::string get_heuristic_search_name() const override { return ""; }

    std::unique_ptr<TaskMDPEngineInterface> create_engine() override
    {
        return this->template create_quotient_heuristic_search_engine<Engine>(
            stop_consistent_,
            reexpand_traps_,
            successor_sampler_.get());
    }

protected:
    void print_additional_statistics() const override
    {
        successor_sampler_->print_statistics(std::cout);
        MDPHeuristicSearch<false, true>::print_additional_statistics();
    }
};

class TrapAwareLRTDPSolverFeature
    : public plugins::TypedFeature<SolverInterface, TrapAwareLRTDPSolver> {
public:
    TrapAwareLRTDPSolverFeature()
        : plugins::TypedFeature<SolverInterface, TrapAwareLRTDPSolver>(
              "talrtdp")
    {
        document_title("Trap-aware LRTDP.");
        document_synopsis(
            "Supports all MDP types (even non-SSPs) without FRET loop.");

        MDPHeuristicSearchBase::add_options_to_feature(*this);

        add_option<std::shared_ptr<TaskSuccessorSamplerFactory>>(
            "successor_sampler",
            "Successor bias for the trials.",
            "random_successor_sampler_factory");
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

static plugins::FeaturePlugin<TrapAwareLRTDPSolverFeature> _plugin;

static plugins::TypedEnumPlugin<TrialTerminationCondition> _enum_plugin(
    {{"terminal", "Stop trials at terminal states"},
     {"consistent", "Stop trials at epsilon consistent states"},
     {"inconsistent", "Stop trials at epsilon inconsistent states"},
     {"revisited", "Stop trials upon revisiting a state"}});

} // namespace
} // namespace solvers
} // namespace probfd
