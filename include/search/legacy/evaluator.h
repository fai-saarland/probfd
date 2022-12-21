#ifndef LEGACY_EVALUATOR_H
#define LEGACY_EVALUATOR_H

#include <set>

namespace legacy {
class GlobalOperator;
class GlobalState;
class Heuristic;

class Evaluator {
public:
    virtual ~Evaluator() {}

    virtual void evaluate(int g, bool preferred) = 0;
    virtual bool is_dead_end() const = 0;
    virtual bool dead_end_is_reliable() const = 0;
    virtual void get_involved_heuristics(std::set<Heuristic*>& hset) = 0;
    virtual int get_value() const = 0;
};
} // namespace legacy
#endif
