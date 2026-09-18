
#ifndef PROBFD_ALGORITHMS_EVENTS_H
#define PROBFD_ALGORITHMS_EVENTS_H

namespace downward {
class State;
}

namespace probfd::algorithms {

struct StateExpansion {
    const downward::State& state;
};

struct GoalStateExpansion {
    const downward::State& state;
};

struct TerminalStateExpansion {
    const downward::State& state;
};

struct PruneStateExpansion {
    const downward::State& state;
};

struct AlgorithmStarted {

};

struct AlgorithmIterationFinished {

};

struct AlgorithmEnded {

};

}

#endif // PROBFD_ALGORITHMS_EVENTS_H
