#ifndef IPC_MAX_HEURISTIC_H
#define IPC_MAX_HEURISTIC_H

#include "heuristic.h"

#include <vector>
#include <memory>

class IPCMaxHeuristic : public Heuristic {
    std::vector<std::shared_ptr<Heuristic> > evaluators;
    int value;
    bool dead_end;
    bool dead_end_reliable;

protected:
    virtual int compute_heuristic(const GlobalState &state);

public:
    IPCMaxHeuristic(const options::Options &options);
    ~IPCMaxHeuristic();
    virtual bool reach_state(const GlobalState &parent_state, const GlobalOperator &op,
                             const GlobalState &state);
};

#endif
