#include "probfd/engine_interfaces/transition_sampler.h"
#include "probfd/engines/trap_aware_lrtdp.h"
#include "probfd/solvers/mdp_heuristic_search.h"
#include "probfd/transition_samplers/task_transition_sampler_factory.h"

#include "option_parser.h"
#include "plugin.h"

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
    WrappedType<std::shared_ptr<TaskTransitionSampler>> successor_sampler_;

public:
    explicit TrapAwareLRTDPSolver(const options::Options& opts)
        : MDPHeuristicSearch<false, true>(opts)
        , stop_consistent_(
              opts.get<TrialTerminationCondition>("terminate_trial"))
        , reexpand_traps_(opts.get<bool>("reexpand_traps"))
        , successor_sampler_(this->wrap(
              opts.get<std::shared_ptr<TaskTransitionSamplerFactory>>(
                      "successor_sampler")
                  ->create_sampler(&this->state_space_)))
    {
    }

    static void add_options_to_parser(options::OptionParser& parser)
    {
        MDPHeuristicSearchBase::add_options_to_parser(parser);
        parser.document_synopsis(
            "Trap-aware LRTDP. Supports all MDP types "
            "(even non-SSPs) without FRET loop.",
            "");
        parser.add_option<std::shared_ptr<TaskTransitionSamplerFactory>>(
            "successor_sampler",
            "Successor bias for the trials.",
            "random_successor_sampler_factory");
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

    virtual engines::MDPEngineInterface<State, OperatorID>*
    create_engine() override
    {
        return this->template create_quotient_heuristic_search_engine<Engine>(
            stop_consistent_,
            reexpand_traps_,
            successor_sampler_.get());
    }

protected:
    virtual void print_additional_statistics() const override
    {
        successor_sampler_->print_statistics(std::cout);
        MDPHeuristicSearch<false, true>::print_additional_statistics();
    }
};

static Plugin<SolverInterface>
    _plugin("talrtdp", options::parse<SolverInterface, TrapAwareLRTDPSolver>);

} // namespace
} // namespace solvers
} // namespace probfd
