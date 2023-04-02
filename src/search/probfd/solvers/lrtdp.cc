#include "probfd/solvers/mdp_heuristic_search.h"

#include "probfd/engines/lrtdp.h"

#include "probfd/engine_interfaces/successor_sampler.h"
#include "probfd/successor_samplers/task_successor_sampler_factory.h"

#include "option_parser.h"
#include "plugin.h"

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
    WrappedType<std::shared_ptr<TaskSuccessorSampler>> successor_sampler_;

public:
    explicit LRTDPSolver(const options::Options& opts)
        : MDPHeuristicSearch<Bisimulation, Fret>(opts)
        , stop_consistent_(
              opts.get<TrialTerminationCondition>("terminate_trial"))
        , successor_sampler_(
              this->wrap(opts.get<std::shared_ptr<TaskSuccessorSamplerFactory>>(
                                 "successor_sampler")
                             ->create_sampler(this->state_space_.get())))
    {
        if constexpr (Fret) {
            if (stop_consistent_ != TrialTerminationCondition::CONSISTENT) {
                std::cout << std::endl;
                std::cout
                    << "Warning: LRTDP is run within FRET without "
                       "stop_consistent being enabled! LRTDP's trials may "
                       "get stuck in cycles."
                    << std::endl;
            }
        }
    }

    virtual std::string get_heuristic_search_name() const override
    {
        return "lrtdp";
    }

    virtual engines::MDPEngineInterface<State, OperatorID>*
    create_engine() override
    {
        return this->template create_heuristic_search_engine<LRTDP>(
            stop_consistent_,
            successor_sampler_.get());
    }

protected:
    virtual void print_additional_statistics() const override
    {
        successor_sampler_->print_statistics(std::cout);
        MDPHeuristicSearch<Bisimulation, Fret>::print_additional_statistics();
    }
};

struct LRTDPOptions {
    void operator()(options::OptionParser& parser) const
    {
        parser.add_option<std::shared_ptr<TaskSuccessorSamplerFactory>>(
            "successor_sampler",
            "",
            "random_successor_sampler_factory");
        {
            std::vector<std::string> opts(
                {"terminal", "consistent", "inconsistent", "revisited"});
            parser.add_enum_option<TrialTerminationCondition>(
                "terminate_trial",
                opts,
                "",
                "terminal");
        }
    }
};

static Plugin<SolverInterface> _plugin(
    "lrtdp",
    parse_mdp_heuristic_search_solver<LRTDPSolver, LRTDPOptions>);

} // namespace
} // namespace solvers
} // namespace probfd
