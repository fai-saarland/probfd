#include "../engines/lrtdp.h"

#include "../../option_parser.h"
#include "../../plugin.h"
#include "../logging.h"
#include "../transition_sampler.h"
#include "mdp_heuristic_search.h"

namespace probfd {
namespace solvers {

template <typename Bisimulation, typename Fret>
class LRTDPSolver : public MDPHeuristicSearch<Bisimulation, Fret> {
public:
    template <typename T>
    using WrappedType =
        typename MDPHeuristicSearch<Bisimulation, Fret>::template WrappedType<
            T>;

    template <typename State, typename Action, typename Bounds>
    using LRTDP = engines::lrtdp::LRTDP<State, Action, Bounds, Fret>;

    explicit LRTDPSolver(const options::Options& opts)
        : MDPHeuristicSearch<Bisimulation, Fret>(opts)
        , stop_consistent_(engines::lrtdp::TrialTerminationCondition(
              opts.get_enum("terminate_trial")))
        , successor_sampler_(this->wrap(
              opts.get<std::shared_ptr<ProbabilisticOperatorTransitionSampler>>(
                  "successor_sampler")))
    {
        if (Fret::value &&
            stop_consistent_ !=
                engines::lrtdp::TrialTerminationCondition::Consistent) {
            logging::out << std::endl;
            logging::out << "Warning: LRTDP is run within FRET without "
                            "stop_consistent being enabled! LRTDP's trials may "
                            "get stuck in cycles."
                         << std::endl;
        }
        this->initialize_interfaceable(this->unwrap(successor_sampler_));
    }

    virtual std::string get_heuristic_search_name() const override
    {
        return "lrtdp";
    }

    virtual engines::MDPEngineInterface<GlobalState>* create_engine() override
    {
        return this->template heuristic_search_engine_factory<LRTDP>(
            stop_consistent_,
            successor_sampler_.get());
    }

protected:
    virtual void print_additional_statistics() const override
    {
        auto ts = this->unwrap(successor_sampler_);
        if (ts != nullptr) {
            ts->print_statistics(logging::out);
        }
        MDPHeuristicSearch<Bisimulation, Fret>::print_additional_statistics();
    }

    const engines::lrtdp::TrialTerminationCondition stop_consistent_;
    WrappedType<std::shared_ptr<ProbabilisticOperatorTransitionSampler>>
        successor_sampler_;
};

struct LRTDPOptions {
    void operator()(options::OptionParser& parser) const
    {
        parser.add_option<
            std::shared_ptr<ProbabilisticOperatorTransitionSampler>>(
            "successor_sampler",
            "",
            "random_successor_sampler");
        {
            std::vector<std::string> opts(
                {"false", "consistent", "inconsistent"});
            parser.add_enum_option("terminate_trial", opts, "", "false");
        }
    }
};

static Plugin<SolverInterface> _plugin(
    "lrtdp",
    parse_mdp_heuristic_search_solver<LRTDPSolver, LRTDPOptions>);

} // namespace solvers
} // namespace probfd
