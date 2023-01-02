#include "probfd/solvers/mdp_heuristic_search.h"
#include "probfd/engines/trap_aware_lrtdp.h"
#include "probfd/transition_sampler.h"
#include "probfd/utils/logging.h"

#include "option_parser.h"
#include "plugin.h"

namespace probfd {
namespace solvers {

using namespace engine_interfaces;
using namespace engines::trap_aware_lrtdp;

class TrapAwareLRTDPSolver
    : public MDPHeuristicSearch<std::false_type, std::true_type> {
public:
    template <typename T>
    using WrappedType =
        typename MDPHeuristicSearch<std::false_type, std::true_type>::
            WrappedType<T>;

    template <typename State, typename Action, typename Bounds>
    using Engine = LRTDP<State, Action, Bounds>;

    explicit TrapAwareLRTDPSolver(const options::Options& opts)
        : MDPHeuristicSearch<std::false_type, std::true_type>(opts)
        , stop_consistent_(
              opts.get<TrialTerminationCondition>("terminate_trial"))
        , reexpand_traps_(opts.get<bool>("reexpand_traps"))
        , successor_sampler_(this->wrap(
              opts.get<std::shared_ptr<ProbabilisticOperatorTransitionSampler>>(
                  "successor_sampler")))
    {
        this->initialize_interfaceable(this->unwrap(successor_sampler_));
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        MDPHeuristicSearchBase::add_options_to_parser(parser);
        parser.document_synopsis(
            "Trap-aware LRTDP. Supports all MDP types "
            "(even non-SSPs) without FRET loop.",
            "");
        parser.add_option<
            std::shared_ptr<ProbabilisticOperatorTransitionSampler>>(
            "successor_sampler",
            "Successor bias for the trials.",
            "random_successor_sampler");
        {
            std::vector<std::string> terminate_trial{
                "disabled",
                "consistent",
                "inconsistent",
                "doublyvisited"};
            parser.add_enum_option<TrialTerminationCondition>(
                "terminate_trial",
                terminate_trial,
                "Stop trials at epsilon-consistent/inconsistent or doubly "
                "visited states.",
                "consistent");
        }
        parser.add_option<bool>(
            "reexpand_traps",
            "Immediately re-expand the collapsed trap state.",
            "true");
    }

    virtual std::string get_engine_name() const override { return "talrtdp"; }

    virtual std::string get_heuristic_search_name() const override
    {
        return "";
    }

    virtual engines::MDPEngineInterface<State>* create_engine() override
    {
        return this->template quotient_heuristic_search_factory<Engine>(
            stop_consistent_,
            reexpand_traps_,
            successor_sampler_.get());
    }

protected:
    virtual void print_additional_statistics() const override
    {
        auto s = this->unwrap(successor_sampler_);
        if (s != nullptr) {
            s->print_statistics(logging::out);
        }
        MDPHeuristicSearch<std::false_type, std::true_type>::
            print_additional_statistics();
    }

    const TrialTerminationCondition stop_consistent_;
    const bool reexpand_traps_;
    WrappedType<std::shared_ptr<ProbabilisticOperatorTransitionSampler>>
        successor_sampler_;
};

static Plugin<SolverInterface>
    _plugin("talrtdp", options::parse<SolverInterface, TrapAwareLRTDPSolver>);

} // namespace solvers
} // namespace probfd
