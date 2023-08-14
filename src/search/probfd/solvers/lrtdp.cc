#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/engines/lrtdp.h"

#include "probfd/engine_interfaces/successor_sampler.h"
#include "probfd/successor_samplers/task_successor_sampler_factory.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace engine_interfaces;
using namespace engines::lrtdp;

template <bool Bisimulation, bool Fret>
class LRTDPSolver : public MDPHeuristicSearch<Bisimulation, Fret> {
    template <typename T>
    using WrappedType =
        typename MDPHeuristicSearch<Bisimulation, Fret>::template WrappedType<
            T>;

    template <typename State, typename Action, bool Interval>
    using LRTDP = LRTDP<State, Action, Interval, Fret>;

    const TrialTerminationCondition stop_consistent_;
    WrappedType<std::shared_ptr<FDRSuccessorSampler>> successor_sampler_;

public:
    explicit LRTDPSolver(const plugins::Options& opts)
        : MDPHeuristicSearch<Bisimulation, Fret>(opts)
        , stop_consistent_(
              opts.get<TrialTerminationCondition>("terminate_trial"))
        , successor_sampler_(
              this->wrap(opts.get<std::shared_ptr<FDRSuccessorSamplerFactory>>(
                                 "successor_sampler")
                             ->create_sampler(this->task_mdp.get())))
    {
        using enum TrialTerminationCondition;
        if constexpr (Fret) {
            if (stop_consistent_ != CONSISTENT &&
                stop_consistent_ != REVISITED) {
                std::cout << std::endl;
                std::cout << "Warning: LRTDP is run within FRET with an unsafe "
                             "trial termination condition! LRTDP's trials may "
                             "get stuck in cycles."
                          << std::endl;
            }
        }
    }

    std::string get_heuristic_search_name() const override { return "lrtdp"; }

    std::unique_ptr<FDRMDPEngine> create_engine() override
    {
        return this->template create_heuristic_search_engine<LRTDP>(
            stop_consistent_,
            successor_sampler_);
    }

protected:
    void print_additional_statistics() const override
    {
        successor_sampler_->print_statistics(std::cout);
        MDPHeuristicSearch<Bisimulation, Fret>::print_additional_statistics();
    }
};

class LRTDPSolverFeature
    : public MDPFRETHeuristicSearchSolverFeature<LRTDPSolver> {
public:
    LRTDPSolverFeature()
        : MDPFRETHeuristicSearchSolverFeature<LRTDPSolver>("lrtdp")
    {
        add_option<std::shared_ptr<FDRSuccessorSamplerFactory>>(
            "successor_sampler",
            "",
            "random_successor_sampler_factory()");

        add_option<TrialTerminationCondition>(
            "terminate_trial",
            "",
            "terminal");
    }
};

static plugins::FeaturePlugin<LRTDPSolverFeature> _plugin;

static plugins::TypedEnumPlugin<TrialTerminationCondition> _enum_plugin(
    {{"terminal", "Stop trials at terminal states"},
     {"consistent", "Stop trials at epsilon consistent states"},
     {"inconsistent", "Stop trials at epsilon inconsistent states"},
     {"revisited", "Stop trials upon revisiting a state"}});

} // namespace
} // namespace solvers
} // namespace probfd
