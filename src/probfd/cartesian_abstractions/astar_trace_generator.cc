#include "probfd/cartesian_abstractions/astar_trace_generator.h"

#include "probfd/cartesian_abstractions/cartesian_abstraction.h"
#include "probfd/cartesian_abstractions/heuristics.h"
#include "probfd/cartesian_abstractions/probabilistic_transition.h"
#include "probfd/cartesian_abstractions/probabilistic_transition_system.h"
#include "probfd/cartesian_abstractions/trace_generator.h"

#include "probfd/utils/guards.h"

#include "downward/utils/countdown_timer.h"

#include <cassert>
#include <deque>
#include <ostream>

using namespace std;

namespace probfd::cartesian_abstractions {

std::ostream& operator<<(std::ostream& os, const TransitionOutcome& t)
{
    return os << "[" << t.op_id << "," << t.eff_id << "," << t.target_id << "]";
}

class AStarTraceGenerator::AbstractSearchInfo {
    value_t g = INFINITE_VALUE;
    TransitionOutcome incoming_transition =
        TransitionOutcome(UNDEFINED, UNDEFINED, UNDEFINED);

public:
    void reset()
    {
        g = INFINITE_VALUE;
        incoming_transition =
            TransitionOutcome(UNDEFINED, UNDEFINED, UNDEFINED);
    }

    void decrease_g_value_to(value_t new_g)
    {
        assert(new_g <= g);
        g = new_g;
    }

    [[nodiscard]]
    value_t get_g_value() const
    {
        return g;
    }

    void set_incoming_transition(const TransitionOutcome& transition)
    {
        incoming_transition = transition;
    }

    [[nodiscard]]
    const TransitionOutcome& get_incoming_transition() const
    {
        assert(
            incoming_transition !=
            TransitionOutcome(UNDEFINED, UNDEFINED, UNDEFINED));
        return incoming_transition;
    }
};

AStarTraceGenerator::AStarTraceGenerator()
    : search_info_(1)
{
}

unique_ptr<Trace> AStarTraceGenerator::find_trace(
    CartesianAbstraction& abstraction,
    int init_id,
    CartesianHeuristic& heuristic,
    utils::CountdownTimer& timer)
{
    // Exception safety (timeout)
    scope_exit guard([&] {
        open_queue_.clear();

        for (AbstractSearchInfo& info : search_info_) {
            info.reset();
        }
    });

    const auto& out =
        abstraction.get_transition_system().get_outgoing_transitions();

    search_info_[init_id].decrease_g_value_to(0);
    open_queue_.push(heuristic.get_h_value(init_id), init_id);

    while (!open_queue_.empty()) {
        timer.throw_if_expired();

        const auto [old_f, state_id] = open_queue_.pop();
        const value_t g = search_info_[state_id].get_g_value();

        assert(0 <= g && g < INFINITE_VALUE);

        value_t new_f = g + heuristic.get_h_value(state_id);

        assert(new_f <= old_f);

        if (new_f < old_f) continue;

        if (abstraction.get_goals().contains(state_id)) {
            open_queue_.clear();
            std::unique_ptr<Trace> solution =
                extract_solution(init_id, state_id, timer);
            update_heuristic(abstraction, heuristic, *solution);
            return solution;
        }

        for (const ProbabilisticTransition* transition : out[state_id]) {
            for (size_t i = 0; i != transition->target_ids.size(); ++i) {
                int op_id = transition->op_id;
                int succ_id = transition->target_ids[i];

                const value_t op_cost = abstraction.get_cost(op_id);
                assert(op_cost >= 0);
                const value_t succ_g = g + op_cost;
                assert(succ_g >= 0);

                if (succ_g < search_info_[succ_id].get_g_value()) {
                    search_info_[succ_id].decrease_g_value_to(succ_g);
                    auto h = heuristic.get_h_value(succ_id);
                    if (h == INFINITE_VALUE) continue;
                    const value_t f = succ_g + h;
                    assert(f >= 0);
                    assert(f != INFINITE_VALUE);
                    open_queue_.push(f, succ_id);
                    search_info_[succ_id].set_incoming_transition(
                        TransitionOutcome(op_id, i, state_id));
                }
            }
        }
    }

    heuristic.set_h_value(init_id, INFINITE_VALUE);

    return nullptr;
}

unique_ptr<Trace> AStarTraceGenerator::extract_solution(
    int init_id,
    int goal_id,
    utils::CountdownTimer& timer) const
{
    unique_ptr<Trace> solution = std::make_unique<Trace>();
    int current_id = goal_id;
    while (current_id != init_id) {
        timer.throw_if_expired();
        const TransitionOutcome& prev =
            search_info_[current_id].get_incoming_transition();
        solution->emplace_front(prev.op_id, prev.eff_id, current_id);
        assert(prev.target_id != current_id);
        current_id = prev.target_id;
    }

    return solution;
}

void AStarTraceGenerator::update_heuristic(
    CartesianAbstraction& abstraction,
    CartesianHeuristic& heuristic,
    const Trace& solution) const
{
    // Update the heuristic
    value_t solution_cost = 0;
    for (const TransitionOutcome& transition : solution) {
        solution_cost += abstraction.get_cost(transition.op_id);
    }

    for (int i = 0; i != abstraction.get_num_states(); ++i) {
        auto& info = search_info_[i];
        if (info.get_g_value() < INFINITE_VALUE) {
            const value_t new_h = std::max(
                heuristic.get_h_value(i),
                solution_cost - info.get_g_value());
            heuristic.set_h_value(i, new_h);
        }
    }
}

void AStarTraceGenerator::notify_split()
{
    search_info_.emplace_back();
}

} // namespace probfd::cartesian_abstractions
