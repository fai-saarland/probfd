#include "probfd/heuristics/cartesian/astar_flaw_generator.h"

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

#include "probfd/utils/guards.h"

#include "cegar/abstract_state.h"

#include "utils/countdown_timer.h"
#include "utils/memory.h"
#include "utils/timer.h"

#include "plugins/plugin.h"

#include <cassert>

using namespace std;

namespace probfd {
namespace heuristics {
namespace cartesian {

AStarFlawGenerator::AStarFlawGenerator()
    : search_info(1)
{
}

optional<Flaw> AStarFlawGenerator::generate_flaw(
    const ProbabilisticTaskProxy& task_proxy,
    Abstraction& abstraction,
    CartesianCostFunction& cost_function,
    const AbstractState* init,
    utils::LogProxy& log,
    const std::vector<int>& domain_sizes,
    utils::Timer& find_trace_timer,
    utils::Timer& find_flaw_timer,
    utils::CountdownTimer& timer,
    int)
{
    // Exception safety (timeout)
    scope_exit guard([&] {
        find_trace_timer.stop();
        find_flaw_timer.stop();

        open_queue.clear();

        for (AbstractSearchInfo& info : search_info) {
            info.reset();
        }
    });

    find_trace_timer.resume();

    const int init_id = init->get_id();
    const int goal_id =
        astar_search(abstraction, cost_function, init_id, timer);

    if (goal_id != UNDEFINED) {
        unique_ptr<Trace> solution = extract_solution(init_id, goal_id, timer);

        value_t solution_cost = 0;
        for (const Transition& transition : *solution) {
            solution_cost += cost_function.get_cost(transition.op_id);
        }

        for (int i = 0; i != abstraction.get_num_states(); ++i) {
            auto& info = search_info[i];
            if (info.get_g_value() < INFINITE_VALUE) {
                const value_t new_h = std::max(
                    heuristic.get_h_value(i),
                    solution_cost - info.get_g_value());
                heuristic.set_h_value(i, new_h);
            }
        }

        find_trace_timer.stop();

        find_flaw_timer.resume();

        optional<Flaw> flaw = find_flaw(
            task_proxy,
            *solution,
            abstraction,
            log,
            domain_sizes,
            timer);

        find_flaw_timer.stop();

        if (!flaw && log.is_at_least_normal()) {
            log << "Found a plan without a flaw in the determinized problem."
                << endl;
        }

        return flaw;
    }

    find_trace_timer.stop();

    if (log.is_at_least_normal()) {
        log << "Abstract task is unsolvable." << endl;
    }

    heuristic.set_h_value(init_id, INFINITE_VALUE);

    return std::nullopt;
}

int AStarFlawGenerator::astar_search(
    Abstraction& abstraction,
    CartesianCostFunction& cost_function,
    int init_id,
    utils::CountdownTimer& timer)
{
    const auto& out =
        abstraction.get_transition_system().get_outgoing_transitions();

    search_info[init_id].decrease_g_value_to(0);
    open_queue.push(heuristic.get_h_value(init_id), init_id);

    while (!open_queue.empty()) {
        timer.throw_if_expired();

        const auto [old_f, state_id] = open_queue.pop();
        const value_t g = search_info[state_id].get_g_value();

        assert(0 <= g && g < INFINITE_VALUE);

        value_t new_f = g + heuristic.get_h_value(state_id);

        assert(new_f <= old_f);

        if (new_f < old_f) continue;

        if (abstraction.get_goals().contains(state_id)) {
            open_queue.clear();
            return state_id;
        }

        for (const ProbabilisticTransition* transition : out[state_id]) {
            for (size_t i = 0; i != transition->target_ids.size(); ++i) {
                int op_id = transition->op_id;
                int succ_id = transition->target_ids[i];

                const value_t op_cost = cost_function.get_cost(op_id);
                assert(op_cost >= 0);
                const value_t succ_g = g + op_cost;
                assert(succ_g >= 0);

                if (succ_g < search_info[succ_id].get_g_value()) {
                    search_info[succ_id].decrease_g_value_to(succ_g);
                    auto h = heuristic.get_h_value(succ_id);
                    if (h == INFINITE_VALUE) continue;
                    const value_t f = succ_g + h;
                    assert(f >= 0);
                    assert(f != INFINITE_VALUE);
                    open_queue.push(f, succ_id);
                    search_info[succ_id].set_incoming_transition(
                        Transition(op_id, i, state_id));
                }
            }
        }
    }

    return UNDEFINED;
}

unique_ptr<AStarFlawGenerator::Trace> AStarFlawGenerator::extract_solution(
    int init_id,
    int goal_id,
    utils::CountdownTimer& timer) const
{
    unique_ptr<Trace> solution = std::make_unique<Trace>();
    int current_id = goal_id;
    while (current_id != init_id) {
        timer.throw_if_expired();
        const Transition& prev =
            search_info[current_id].get_incoming_transition();
        solution->emplace_front(prev.op_id, prev.eff_id, current_id);
        assert(prev.target_id != current_id);
        current_id = prev.target_id;
    }
    return solution;
}

optional<Flaw> AStarFlawGenerator::find_flaw(
    const ProbabilisticTaskProxy& task_proxy,
    const AStarFlawGenerator::Trace& solution,
    Abstraction& abstraction,
    utils::LogProxy& log,
    const std::vector<int>& domain_sizes,
    utils::CountdownTimer& timer)
{
    if (log.is_at_least_debug()) log << "Check solution:" << endl;

    const AbstractState* abstract_state = &abstraction.get_initial_state();
    State concrete_state = task_proxy.get_initial_state();
    assert(abstract_state->includes(concrete_state));

    if (log.is_at_least_debug())
        log << "  Initial abstract state: " << *abstract_state << endl;

    for (const Transition& step : solution) {
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
                concrete_state.get_unregistered_successor(outcome);
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

void AStarFlawGenerator::notify_split(int v)
{
    // Update heuristic
    heuristic.on_split(v);

    search_info.emplace_back();
}

CartesianHeuristic& AStarFlawGenerator::get_heuristic()
{
    return heuristic;
}

bool AStarFlawGenerator::is_complete()
{
    return false;
}

std::unique_ptr<FlawGenerator>
AStarFlawGeneratorFactory::create_flaw_generator() const
{
    return std::make_unique<AStarFlawGenerator>();
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
} // namespace heuristics
} // namespace probfd