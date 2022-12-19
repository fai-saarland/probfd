#ifndef GOAL_COUNT_HEURISTIC_H
#define GOAL_COUNT_HEURISTIC_H

#include "heuristic.h"

class GoalCountHeuristic : public Heuristic {
protected:
    virtual void initialize();
    virtual int compute_heuristic(const GlobalState &state);
public:
    GoalCountHeuristic(const options::Options &options);
    ~GoalCountHeuristic();
};

#endif
