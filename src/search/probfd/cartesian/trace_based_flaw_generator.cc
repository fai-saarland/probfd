#include "probfd/cartesian/trace_based_flaw_generator.h"

#include "probfd/cartesian/abstract_state.h"
#include "probfd/cartesian/abstraction.h"
#include "probfd/cartesian/astar_trace_generator.h"
#include "probfd/cartesian/evaluators.h"
#include "probfd/cartesian/probabilistic_transition_system.h"
#include "probfd/cartesian/split_selector.h"
#include "probfd/cartesian/trace_generator.h"
#include "probfd/cartesian/utils.h"

#include "probfd/task_utils/task_properties.h"

#include "probfd/utils/guards.h"

#include "downward/utils/countdown_timer.h"
#include "downward/utils/memory.h"
#include "downward/utils/timer.h"

#include "downward/plugins/plugin.h"

#include <cassert>

using namespace std;

namespace probfd {
namespace cartesian {

TraceBasedFlawGenerator::TraceBasedFlawGenerator(
    TraceGenerator* trace_generator)
    : trace_generator(trace_generator)
{
}

TraceBasedFlawGenerator::~TraceBasedFlawGenerator() = default;

std::unique_ptr<Trace> TraceBasedFlawGenerator::find_trace(
    Abstraction& abstraction,
    int init_id,
    CartesianHeuristic& heuristic,
    utils::CountdownTimer& timer)
{
    TimerScope scope(find_trace_timer);
    return trace_generator->find_trace(abstraction, init_id, heuristic, timer);
}

optional<Flaw> TraceBasedFlawGenerator::generate_flaw(
    const ProbabilisticTaskProxy& task_proxy,
    const std::vector<int>& domain_sizes,
    Abstraction& abstraction,
    const AbstractState* init,
    CartesianHeuristic& heuristic,
    utils::LogProxy& log,
    utils::CountdownTimer& timer)
{
    std::unique_ptr<Trace> solution =
        find_trace(abstraction, init->get_id(), heuristic, timer);

    if (!solution) {
        if (log.is_at_least_normal()) {
            log << "Abstract task is unsolvable." << endl;
        }

        return std::nullopt;
    }

    optional<Flaw> flaw =
        find_flaw(task_proxy, domain_sizes, *solution, abstraction, log, timer);

    if (!flaw && log.is_at_least_normal()) {
        log << "Found a plan without a flaw in the determinized problem."
            << endl;
    }

    return flaw;
}

optional<Flaw> TraceBasedFlawGenerator::find_flaw(
    const ProbabilisticTaskProxy& task_proxy,
    const std::vector<int>& domain_sizes,
    const Trace& solution,
    Abstraction& abstraction,
    utils::LogProxy& log,
    utils::CountdownTimer& timer)
{
    TimerScope scope(find_flaw_timer);

    if (log.is_at_least_debug()) log << "Check solution:" << endl;

    const AbstractState* abstract_state = &abstraction.get_initial_state();
    State concrete_state = task_proxy.get_initial_state();
    assert(abstract_state->includes(concrete_state));

    if (log.is_at_least_debug())
        log << "  Initial abstract state: " << *abstract_state << endl;

    for (const TransitionOutcome& step : solution) {
        timer.throw_if_expired();
        if (!utils::extra_memory_padding_is_reserved()) break;
        ProbabilisticOperatorProxy op = task_proxy.get_operators()[step.op_id];
        const AbstractState* next_abstract_state =
            &abstraction.get_abstract_state(step.target_id);
        if (::task_properties::is_applicable(op, concrete_state)) {
            if (log.is_at_least_debug())
                log << "  Move to " << *next_abstract_state << " with "
                    << op.get_name() << endl;
            const auto outcome = op.get_outcomes()[step.eff_id];
            State next_concrete_state =
                concrete_state.get_unregistered_successor(
                    outcome.get_effects());
            if (!next_abstract_state->includes(next_concrete_state)) {
                if (log.is_at_least_debug()) log << "  Paths deviate." << endl;
                return Flaw(
                    std::move(concrete_state),
                    *abstract_state,
                    next_abstract_state->regress(op, outcome.get_effects()));
            }
            abstract_state = next_abstract_state;
            concrete_state = std::move(next_concrete_state);
        } else {
            if (log.is_at_least_debug())
                log << "  Operator not applicable: " << op.get_name() << endl;
            return Flaw(
                std::move(concrete_state),
                *abstract_state,
                get_cartesian_set(domain_sizes, op.get_preconditions()));
        }
    }

    assert(abstraction.get_goals().contains(abstract_state->get_id()));
    if (::task_properties::is_goal_state(task_proxy, concrete_state)) {
        // We found a concrete solution.
        return std::nullopt;
    }

    if (log.is_at_least_debug()) log << "  Goal test failed." << endl;
    return Flaw(
        std::move(concrete_state),
        *abstract_state,
        get_cartesian_set(domain_sizes, task_proxy.get_goals()));
}

void TraceBasedFlawGenerator::notify_split()
{
    trace_generator->notify_split();
}

void TraceBasedFlawGenerator::print_statistics(utils::LogProxy& log)
{
    if (log.is_at_least_normal()) {
        log << "Time for finding abstract traces: " << find_trace_timer << endl;
        log << "Time for finding trace flaws: " << find_flaw_timer << endl;
    }
}

std::unique_ptr<FlawGenerator>
AStarFlawGeneratorFactory::create_flaw_generator()
{
    return std::make_unique<TraceBasedFlawGenerator>(new AStarTraceGenerator());
}

class AStarFlawGeneratorFactoryFeature
    : public plugins::
          TypedFeature<FlawGeneratorFactory, AStarFlawGeneratorFactory> {
public:
    AStarFlawGeneratorFactoryFeature()
        : TypedFeature("flaws_astar")
    {
    }

    std::shared_ptr<AStarFlawGeneratorFactory>
    create_component(const plugins::Options&, const utils::Context&)
        const override
    {
        return std::make_shared<AStarFlawGeneratorFactory>();
    }
};

static plugins::FeaturePlugin<AStarFlawGeneratorFactoryFeature> _plugin;

} // namespace cartesian
} // namespace probfd