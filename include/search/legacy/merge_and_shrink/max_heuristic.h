#ifndef LEGACY_MAX_HEURISTIC_H
#define LEGACY_MAX_HEURISTIC_H

#include "algorithms/priority_queues.h"

#include "legacy/merge_and_shrink/relaxation_heuristic.h"

#include <cassert>

namespace legacy {
namespace merge_and_shrink {

class HSPMaxHeuristic : public RelaxationHeuristic {
    priority_queues::AdaptiveQueue<Proposition *> queue;

    void setup_exploration_queue();
    void setup_exploration_queue_state(const GlobalState &state);
    void relaxed_exploration();

    void enqueue_if_necessary(Proposition *prop, int cost) {
        assert(cost >= 0);
        if (prop->cost == -1 || prop->cost > cost) {
            prop->cost = cost;
            queue.push(cost, prop);
        }
        assert(prop->cost != -1 && prop->cost <= cost);
    }
protected:
    virtual void initialize();
    virtual int compute_heuristic(const GlobalState &state);
public:
    HSPMaxHeuristic(const options::Options &options);
    ~HSPMaxHeuristic();
};

}
}

#endif
