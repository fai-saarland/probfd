
#ifndef MS_EVENTS_H
#define MS_EVENTS_H

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

#endif // MS_EVENTS_H
