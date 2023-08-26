#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/algorithms/lrtdp.h"

#include "probfd/algorithms/successor_sampler.h"

#include "probfd/plugins/multi_feature_plugin.h"
#include "probfd/plugins/naming_conventions.h"

#include "downward/plugins/plugin.h"

namespace probfd {
namespace solvers {
namespace {

using namespace algorithms;
using namespace algorithms::lrtdp;

using namespace plugins;

template <bool Bisimulation, bool Fret>
using Sampler = std::conditional_t<
    Bisimulation,
    std::conditional_t<
        Fret,
        SuccessorSampler<
            quotients::QuotientAction<bisimulation::QuotientAction>>,
        SuccessorSampler<bisimulation::QuotientAction>>,
    std::conditional_t<
        Fret,
        SuccessorSampler<quotients::QuotientAction<OperatorID>>,
        SuccessorSampler<OperatorID>>>;

template <bool Bisimulation, bool Fret>
class LRTDPSolver : public MDPHeuristicSearch<Bisimulation, Fret> {
    template <typename State, typename Action, bool Interval>
    using LRTDP = LRTDP<State, Action, Interval, Fret>;

    using Sampler = Sampler<Bisimulation, Fret>;

    const TrialTerminationCondition stop_consistent_;
    const std::shared_ptr<Sampler> successor_sampler_;

public:
    explicit LRTDPSolver(const Options& opts)
        : MDPHeuristicSearch<Bisimulation, Fret>(opts)
        , stop_consistent_(
              opts.get<TrialTerminationCondition>("terminate_trial"))
        , successor_sampler_(
              opts.get<std::shared_ptr<Sampler>>("successor_sampler"))
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

    std::unique_ptr<FDRMDPAlgorithm> create_algorithm() override
    {
        return this->template create_heuristic_search_algorithm<LRTDP>(
            stop_consistent_,
            successor_sampler_);
    }

protected:
    void print_additional_statistics() const override
    {
        MDPHeuristicSearch<Bisimulation, Fret>::print_additional_statistics();
        successor_sampler_->print_statistics(std::cout);
    }
};

template <bool Bisimulation, bool Fret>
class LRTDPSolverFeature
    : public TypedFeature<SolverInterface, LRTDPSolver<Bisimulation, Fret>> {
public:
    LRTDPSolverFeature()
        : TypedFeature<SolverInterface, LRTDPSolver<Bisimulation, Fret>>(
              add_wrapper_algo_suffix<Bisimulation, Fret>("lrtdp"))
    {
        MDPHeuristicSearch<Bisimulation, Fret>::add_options_to_feature(*this);

        this->template add_option<std::shared_ptr<Sampler<Bisimulation, Fret>>>(
            "successor_sampler",
            "",
            add_mdp_type_to_option<Bisimulation, Fret>(
                "random_successor_sampler()"));

        this->template add_option<TrialTerminationCondition>(
            "terminate_trial",
            "",
            "terminal");
    }
};

static MultiFeaturePlugin<LRTDPSolverFeature> _plugins;

static TypedEnumPlugin<TrialTerminationCondition> _enum_plugin(
    {{"terminal", "Stop trials at terminal states"},
     {"consistent", "Stop trials at epsilon consistent states"},
     {"inconsistent", "Stop trials at epsilon inconsistent states"},
     {"revisited", "Stop trials upon revisiting a state"}});

} // namespace
} // namespace solvers
} // namespace probfd
