#include "probfd/heuristics/cartesian/ilao_flaw_generator.h"

#include "probfd/heuristics/cartesian/engine_interfaces.h"
#include "probfd/heuristics/cartesian/probabilistic_transition_system.h"
#include "probfd/heuristics/cartesian/split_selector.h"
#include "probfd/heuristics/cartesian/utils.h"

#include "probfd/engines/ta_topological_value_iteration.h"
#include "probfd/engines/trap_aware_dfhs.h"

#include "probfd/quotients/engine_interfaces.h"
#include "probfd/quotients/heuristic_search_interface.h"

#include "probfd/preprocessing/qualitative_reachability_analysis.h"

#include "probfd/task_utils/task_properties.h"

#include "cegar/abstract_state.h"

#include "utils/countdown_timer.h"
#include "utils/iterators.h"
#include "utils/memory.h"

#include <cassert>

using namespace std;

namespace probfd {
namespace heuristics {
namespace cartesian {

ILAOFlawGenerator::ILAOFlawGenerator()
    : ptb(new policy_pickers::ArbitraryTiebreaker<
          const AbstractState*,
          const ProbabilisticTransition*>(true))
    , report(0.0_vt)
{
    report.disable();
}

std::optional<Flaw> ILAOFlawGenerator::generate_flaw(
    const ProbabilisticTaskProxy& task_proxy,
    Abstraction& abstraction,
    CartesianCostFunction& cost_function,
    const AbstractState* init,
    utils::LogProxy& log,
    const std::vector<int>& domain_sizes,
    utils::Timer& find_trace_timer,
    utils::Timer& find_flaw_timer,
    utils::CountdownTimer& timer,
    int max_search_states)
{
    find_trace_timer.resume();
    unique_ptr<Solution> solution =
        this->find_solution(abstraction, cost_function, init, timer);
    find_trace_timer.stop();

    if (!solution) {
        if (log.is_at_least_normal()) {
            log << "Abstract task is unsolvable." << endl;
        }
        return std::nullopt;
    }

    find_flaw_timer.resume();
    optional<Flaw> flaw = this->find_flaw(
        task_proxy,
        abstraction,
        *solution,
        timer,
        log,
        domain_sizes,
        max_search_states);
    find_flaw_timer.stop();

    return flaw;
}

unique_ptr<Solution> ILAOFlawGenerator::find_solution(
    Abstraction& abstraction,
    CartesianCostFunction& cost_function,
    const AbstractState* state,
    utils::CountdownTimer& timer)
{
    quotients::
        QuotientSystem<const AbstractState*, const ProbabilisticTransition*>
            quotient(&abstraction);

    quotients::DefaultQuotientCostFunction<
        const AbstractState*,
        const ProbabilisticTransition*>
        costs(&quotient, &cost_function);

    quotients::RepresentativePolicyPicker<
        const AbstractState*,
        const ProbabilisticTransition*>
        picker(&quotient, ptb);

    engines::trap_aware_dfhs::TADepthFirstHeuristicSearch<
        const AbstractState*,
        const ProbabilisticTransition*,
        false>
        hdfs(
            &quotient,
            &costs,
            &heuristic,
            &picker,
            nullptr,
            &report,
            false,
            &quotient,
            false,
            engines::trap_aware_dfhs::BacktrackingUpdateType::SINGLE,
            false,
            false,
            false,
            true,
            false,
            true,
            nullptr);

    auto policy = hdfs.compute_policy(state, timer.get_remaining_time());

    for (int i = 0; i != abstraction.get_num_states(); ++i) {
        if (hdfs.was_visited(i)) {
            heuristic.set_h_value(i, hdfs.lookup_value(i));
        }
    }

    return policy;
}

optional<Flaw> ILAOFlawGenerator::find_flaw(
    const ProbabilisticTaskProxy& task_proxy,
    Abstraction& abstraction,
    Solution& policy,
    utils::CountdownTimer& timer,
    utils::LogProxy& log,
    const std::vector<int>& domain_sizes,
    int max_search_states)
{
    StateRegistry state_registry(task_proxy);

    struct QueueItem {
        ::StateID state_id;
        const AbstractState* abstract_state;
    };

    const State initial = state_registry.get_initial_state();
    const AbstractState* abstract_initial = &abstraction.get_initial_state();

    std::deque<QueueItem> frontier({{initial.get_id(), abstract_initial}});
    storage::PerStateStorage<bool> visited(false);
    visited[0] = true;

    for (; !frontier.empty(); frontier.pop_front()) {
        timer.throw_if_expired();
        if (!utils::extra_memory_padding_is_reserved()) return std::nullopt;

        QueueItem& next = frontier.front();

        State state = state_registry.lookup_state(next.state_id);

        auto decision = policy.get_decision(next.abstract_state);

        // Check for goal state
        if (!decision) {
            assert(abstraction.get_goals().contains(
                next.abstract_state->get_id()));

            if (!::task_properties::is_goal_state(task_proxy, state)) {
                if (log.is_at_least_debug()) log << "Goal test failed." << endl;
                state.unpack();
                return Flaw(
                    std::move(state),
                    *next.abstract_state,
                    get_cartesian_set(domain_sizes, task_proxy.get_goals()));
            }
            continue;
        }

        const ProbabilisticTransition* transition = decision->action;
        const auto op = task_proxy.get_operators()[transition->op_id];

        // Check for operator applicability
        if (!task_properties::is_applicable(op, state)) {
            if (log.is_at_least_debug())
                log << "Operator not applicable: " << op.get_name() << endl;
            state.unpack();
            return Flaw(
                std::move(state),
                *next.abstract_state,
                get_cartesian_set(domain_sizes, op.get_preconditions()));
        }

        // Generate successors and check for matching abstract states
        for (size_t i = 0; i != transition->target_ids.size(); ++i) {
            const auto outcome = op.get_outcomes()[i];
            const AbstractState* next_abstract_state =
                &abstraction.get_abstract_state(transition->target_ids[i]);

            State next_concrete_state =
                state_registry.get_successor_state(state, outcome);

            if (static_cast<int>(state_registry.size()) > max_search_states) {
                if (log.is_at_least_normal()) {
                    log << "Reached maximal number of flaw search states."
                        << endl;
                }
                return std::nullopt;
            }

            if (!next_abstract_state->includes(next_concrete_state)) {
                if (log.is_at_least_debug()) log << "  Paths deviate." << endl;
                state.unpack();
                return Flaw(
                    std::move(state),
                    *next.abstract_state,
                    next_abstract_state->regress(op, outcome.get_effects()));
            }

            // Add successor to frontier if not seen before
            auto bool_proxy = visited[next_concrete_state.get_id()];
            if (!bool_proxy) {
                bool_proxy = true;
                frontier.emplace_back(
                    next_concrete_state.get_id(),
                    next_abstract_state);
            }
        }
    }

    // We found a concrete policy.
    if (log.is_at_least_normal()) {
        log << "Found concrete solution during refinement." << endl;
    }

    return std::nullopt;
}

void ILAOFlawGenerator::notify_split(int v)
{
    heuristic.on_split(v);
}

CartesianHeuristic& ILAOFlawGenerator::get_heuristic()
{
    return heuristic;
}

bool ILAOFlawGenerator::is_complete()
{
    return true;
}

std::unique_ptr<FlawGenerator>
ILAOFlawGeneratorFactory::create_flaw_generator() const
{
    return std::make_unique<ILAOFlawGenerator>();
}

static shared_ptr<FlawGeneratorFactory> _parse(OptionParser& parser)
{
    if (parser.dry_run()) return nullptr;
    return make_shared<ILAOFlawGeneratorFactory>();
}

static Plugin<FlawGeneratorFactory> _plugin("flaws_ilao", _parse);

} // namespace cartesian
} // namespace heuristics
} // namespace probfd