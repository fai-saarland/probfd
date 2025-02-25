
#ifndef MS_EVENTS_H
#define MS_EVENTS_H

class State;

namespace probfd::algorithms {

struct StateExpansion {
    const State& state;
};

struct GoalStateExpansion {
    const State& state;
};

struct TerminalStateExpansion {
    const State& state;
};

struct PruneStateExpansion {
    const State& state;
};

struct AlgorithmStarted {

};

struct AlgorithmIterationFinished {

};

struct AlgorithmEnded {

};

}

#endif // MS_EVENTS_H
