#ifndef PROBFD_HEURISTICS_CARTESIAN_ASTAR_FLAW_GENERATOR_H
#define PROBFD_HEURISTICS_CARTESIAN_ASTAR_FLAW_GENERATOR_H

#include "probfd/heuristics/cartesian/flaw_generator.h"

namespace probfd {
namespace heuristics {
namespace cartesian {

/*
  Find flaws using A* (incomplete).
*/
class AStarFlawGenerator : public FlawGenerator {
    struct Transition {
        int op_id;
        int eff_id;
        int target_id;

        Transition(int op_id, int eff_id, int target_id)
            : op_id(op_id)
            , eff_id(eff_id)
            , target_id(target_id)
        {
        }

        friend bool operator==(const Transition&, const Transition&) = default;

        friend std::ostream& operator<<(std::ostream& os, const Transition& t)
        {
            return os << "[" << t.op_id << "," << t.eff_id << "," << t.target_id
                      << "]";
        }
    };

    using Trace = std::deque<Transition>;

    class AbstractSearchInfo {
        value_t g = INFINITE_VALUE;
        Transition incoming_transition =
            Transition(UNDEFINED, UNDEFINED, UNDEFINED);

    public:
        void reset()
        {
            g = INFINITE_VALUE;
            incoming_transition = Transition(UNDEFINED, UNDEFINED, UNDEFINED);
        }

        void decrease_g_value_to(value_t new_g)
        {
            assert(new_g <= g);
            g = new_g;
        }

        value_t get_g_value() const { return g; }

        void set_incoming_transition(const Transition& transition)
        {
            incoming_transition = transition;
        }

        const Transition& get_incoming_transition() const
        {
            assert(
                incoming_transition !=
                Transition(UNDEFINED, UNDEFINED, UNDEFINED));
            return incoming_transition;
        }
    };

    CartesianHeuristic heuristic;

    // Keep data structures around to avoid reallocating them.
    priority_queues::HeapQueue<value_t, int> open_queue;
    std::vector<AbstractSearchInfo> search_info;

    int astar_search(
        Abstraction& abstraction,
        CartesianCostFunction& cost_function,
        int init_id,
        utils::CountdownTimer& timer);

    std::unique_ptr<Trace>
    extract_solution(int init_id, int goal_id, utils::CountdownTimer& timer)
        const;

    std::optional<Flaw> find_flaw(
        const ProbabilisticTaskProxy& task_proxy,
        const Trace& solution,
        Abstraction& abstraction,
        utils::LogProxy& log,
        const std::vector<int>& domain_sizes,
        utils::CountdownTimer& timer);

public:
    AStarFlawGenerator();

    std::optional<Flaw> generate_flaw(
        const ProbabilisticTaskProxy& task_proxy,
        Abstraction& abstraction,
        CartesianCostFunction& cost_function,
        const AbstractState* init_id,
        utils::LogProxy& log,
        const std::vector<int>& domain_sizes,
        utils::Timer& find_trace_timer,
        utils::Timer& find_flaw_timer,
        utils::CountdownTimer& timer) override;

    void notify_split(int v) override;

    CartesianHeuristic& get_heuristic() override;

    bool is_complete() override final;
};

class AStarFlawGeneratorFactory : public FlawGeneratorFactory {
public:
    std::unique_ptr<FlawGenerator> create_flaw_generator() const override;
};

} // namespace cartesian
} // namespace heuristics
} // namespace probfd

#endif
